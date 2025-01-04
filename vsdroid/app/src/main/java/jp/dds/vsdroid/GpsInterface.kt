package jp.dds.vsdroid

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.SharedPreferences
import android.os.Handler
import android.util.Log
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader
import java.io.OutputStream
import java.net.SocketTimeoutException
import java.util.Calendar
import java.util.TimeZone
import java.util.UUID
import kotlin.concurrent.Volatile
import kotlin.math.floor

//*** VSD アクセス *******************************************************
class GpsInterface(context: Context?, _MsgHandler: Handler?, _Pref: SharedPreferences?) : Runnable {
	var Pref: SharedPreferences?
	var GpsThread: Thread? = null

	// Bluetooth アダプタ
	var device: BluetoothDevice? = null
	var BTSock: BluetoothSocket? = null
	var mBluetoothAdapter: BluetoothAdapter? = null

	var brInput: BufferedReader? = null
	var OutStream: OutputStream? = null

	@Volatile
	var bKillThread: Boolean = false

	var MsgHandler: Handler? = null

	// GPS データ
	var iCurrentYear: Int
	var iNmeaFlag: Int = 0
	var iNmeaTime: Int = -1
	var GpsTime: Calendar
	var dLong: Double = Double.NaN
	var dLati: Double = 0.0
	var dAlt: Double = 0.0
	var dSpeed: Double = 0.0

	var iPrevNmeaTime: Int = -1
	var dPrevLong: Double = Double.NaN
	var dPrevLati: Double = 0.0
	var Connected = false

	//*** リトライしないerror ************************************************

	class UnrecoverableException(message: String?) : Exception(message) {
		companion object {
			private const val serialVersionUID = 1L
		}
	}

	//************************************************************************
	// コンストラクタ - 変数の初期化だけやってる
	init {
		if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::constructor")

		MsgHandler = _MsgHandler
		Pref = _Pref

		val bluetoothManager = context?.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
		mBluetoothAdapter = bluetoothManager.getAdapter()

		// 現在の年取得
		GpsTime = Calendar.getInstance(TimeZone.getTimeZone("GMT+0"))
		iCurrentYear = GpsTime[Calendar.YEAR]
	}

	@Throws(IOException::class, UnrecoverableException::class)
	fun Open() {
		if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Open")
		// If the adapter is null, then Bluetooth is not supported
		if (mBluetoothAdapter == null) {
			//MsgHandler.sendEmptyMessage( R.string.statmsg_bluetooth_na );
			throw UnrecoverableException("Bluetooth error")
		}

		// BT ON でなければエラーで帰る
		if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Enable")
		if (!mBluetoothAdapter!!.isEnabled) {
			throw UnrecoverableException("Bluetooth error")
		}

		// BT の MAC アドレスを求める
		val s = Pref!!.getString("key_bt_devices_gps", null)
			?: //MsgHandler.sendEmptyMessage( R.string.statmsg_bluetooth_dev_not_selected );
			throw UnrecoverableException("Bluetooth error")
		device = mBluetoothAdapter!!.getRemoteDevice(s.substring(s.length - 17))

		try {
			//MsgHandler.sendEmptyMessage( R.string.statmsg_bluetooth_connecting );
			// Get a BluetoothSocket for a connection with the
			// given BluetoothDevice
			if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::createRfcommSocket")
			BTSock = device!!.createInsecureRfcommSocketToServiceRecord(BT_UUID)

			// ソケットの作成 12秒でタイムアウトらしい
			if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Open:connecting...")
			BTSock!!.connect()
			val `in` = BTSock!!.getInputStream()
			brInput = BufferedReader(InputStreamReader(`in`))
			OutStream = BTSock!!.getOutputStream()

			if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Open:connected")
			//MsgHandler.sendEmptyMessage( R.string.statmsg_gps_connected );
		} catch (e: SocketTimeoutException) {
			if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Open:timeout")
			Close()
			throw IOException("Bluetooth error")
		} catch (e: SecurityException) {
			if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Open:SeculityException")
			Close()
			throw e
		} catch (e: IOException) {
			if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Open:IOException")
			Close()
			throw e
		}
	}

	fun Close(): Int {
		if (bDebug) Log.d("VSDroid", "GpsInterface:GpsInterface::Close")

		try {
			if (brInput != null) {
				brInput!!.close()
				brInput = null
			}
		} catch (_: IOException) {}

		try {
			if (OutStream != null) {
				OutStream!!.close()
				OutStream = null
			}
		} catch (_: IOException) {}

		try {
			if (BTSock != null) {
				BTSock!!.close()
				BTSock = null
			}
		} catch (_: IOException) {}

		Connected = false
		dPrevLong = Double.NaN
		dLong = dPrevLong

		return 0
	}

	//*** 1行処理 ************************************************************
	private fun ParseInt(str: String, iDefault: Int): Int {
		try {
			return str.toInt()
		} catch (_: Exception) {}
		return iDefault
	}

	private fun ParseDouble(str: String): Double {
		try {
			return str.toDouble()
		} catch (_: Exception) {}
		return Double.NaN
	}

	private fun ParseTime(str: String): Int {
		return ParseInt(str.substring(0, 2), 0) * 3600 * 1000 + ParseInt(
			str.substring(2, 4),
			0
		) * 60 * 1000 + (ParseDouble(str.substring(4)) * 1000).toInt()
	}

	@Throws(IOException::class)
	fun Read() {
		val str =
			brInput!!.readLine().split(",".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()

		if (str[0] == "\$GPRMC") {
			dPrevLong = dLong
			dPrevLati = dLati
			iPrevNmeaTime = iNmeaTime

			//		時間		 lat		   long		   knot  方位   日付
			// 0	  1		  2 3		   4 5			6 7	 8	  9
			// $GPRMC,043431.200,A,3439.997825,N,13523.377978,E,0.602,178.29,240612,,,A*59
			val iTime = ParseTime(str[1])

			if (iNmeaFlag == 0) {
				iNmeaTime = iTime
				iNmeaFlag = 1
			} else {
				iNmeaFlag = iNmeaFlag or 1
			}

			// Lat
			dLati = ParseDouble(str[3])
			dLati = floor(dLati / 100) + (dLati % 100.0) / 60.0
			if (str[4] == "S") dLati = -dLati

			// Long
			dLong = ParseDouble(str[5])
			dLong = floor(dLong / 100) + (dLong % 100.0) / 60.0
			if (str[6] == "W") dLong = -dLong

			// Speed
			dSpeed = ParseDouble(str[7]) * 1.85200

			// 日付
			var iYear = ParseInt(str[9].substring(4, 6), 0) +
					(iCurrentYear / 100 * 100)

			if ((iCurrentYear % 100) >= 50 && iYear < 50) iYear += 100

			GpsTime[iYear, ParseInt(str[9].substring(2, 4), 1) - 1, ParseInt(
				str[9].substring(0, 2),
				1
			), iTime / (3600 * 1000), iTime / (60 * 1000) % 60] =
				iTime / (1000) % 60
			GpsTime[Calendar.MILLISECOND] = iTime % 1000
		} else if (str[0] == "\$GPGGA") {
			//		時間	   lat		   long			   高度
			// 0	  1		  2		   3 4			5 6 789
			// $GPGGA,233132.000,3439.997825,N,13523.377978,E,1,,,293.425,M,,,,*21

			val iTime = ParseTime(str[1])

			if (iNmeaFlag == 0) {
				iNmeaTime = iTime
				iNmeaFlag = 2
			} else {
				iNmeaFlag = iNmeaFlag or 2
			}

			dAlt = ParseDouble(str[9])
		}

		if (iNmeaFlag == 3) {
			// NMEA データが 1組分揃った

			iNmeaFlag = 0
			Connected = true

			if (!java.lang.Double.isNaN(dLong)) {
				MsgHandler!!.sendEmptyMessage(R.string.statmsg_gps_updated)
			}
		}
	}

	//*** データ処理スレッド *********************************************
	fun Start() {
		if (bDebug) Log.d("VSDroid", "Start()")
		// VSD スレッド起動
		GpsThread = Thread(this)
		GpsThread!!.start()
	}

	override fun run() {
		bKillThread = false

		if (bDebug) Log.d("VSDroid", "GpsInterface:run_loop()")

		MainLoop@ while (!bKillThread) {
			// 開く，失敗したらやり直し

			while (!bKillThread) {
				try {
					if (bDebug) Log.d("VSDroid", "GpsInterface:opening...")
					Close() // 開いていたら一旦 close
					Open()
					break
				} catch (e: UnrecoverableException) {
					// BT off 等，リカバリ不能な問題
					if (bDebug) Log.d("VSDroid", "GpsInterface:Unrecoverable exception")
					break@MainLoop
				} catch (e: IOException) {
					// 一般的な IOException
					if (bDebug) Log.d("VSDroid", "GpsInterface:retrying open...")
					try {
						Thread.sleep((3 * 1000).toLong())
					} catch (_: InterruptedException) {}
				}
			}

			if (bDebug) Log.d("VSDroid", "GpsInterface:open done.")

			// 1行読む
			while (!bKillThread) {
				try {
					Read()
				} catch (e: IOException) {
					break
				}
			}
			
			// ここに来たということは，何らかの理由で GPS 切断
			//MsgHandler.sendEmptyMessage( R.string.statmsg_gps_disconnected );
		}

		if (bDebug) Log.d("VSDroid", "GpsInterface:run() loop extting.")

		Close()

		bKillThread = false
		if (bDebug) Log.d("VSDroid", "GpsInterface:exit_run()")
	}

	fun KillThread() {
		bKillThread = true

		if (bDebug) Log.d("VSDroid", "GpsInterface:KillThread: killing...")
		try {
			while (bKillThread && GpsThread != null && GpsThread!!.isAlive) {
				Thread.sleep(100)
				Close()
			}
		} catch (_: InterruptedException) {}

		GpsThread = null
		if (bDebug) Log.d("VSDroid", "GpsInterface:KillThread: done")

		dPrevLong = Double.NaN
		dLong = dPrevLong
	}

	companion object {
		val bDebug: Boolean = BuildConfig.DEBUG

		private val BT_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
	}
}
