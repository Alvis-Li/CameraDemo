package com.demo.ui;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.PictureCallback;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

public class MainActivity extends Activity {
	protected static final String TAG = "main";
	private Camera mCamera;

	// private CameraPreview mPreview;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		mCamera = getCameraInstance();

		// 创建预览类，并与Camera关联，最后添加到界面布局中
		// mPreview = new CameraPreview(this, mCamera);
		// FrameLayout preview = (FrameLayout)
		// findViewById(R.id.camera_preview);
		// preview.addView(mPreview);

		SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surfaceview);
		FrameLayout layout = (FrameLayout) findViewById(R.id.layout_all);
		int height = getResources().getDisplayMetrics().widthPixels * 480 / 320;
		Log.e(TAG, "height " + height);
		RelativeLayout.LayoutParams params = (android.widget.RelativeLayout.LayoutParams) layout
				.getLayoutParams();
		params.height = height;

		SurfaceHolder holder = surfaceView.getHolder();
		SurfaceHolderCallBack callBack = new SurfaceHolderCallBack(holder,
				mCamera, this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		holder.addCallback(callBack);

		Button captureButton = (Button) findViewById(R.id.camera_take);
		captureButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				// 在捕获图片前进行自动对焦
				mCamera.autoFocus(new AutoFocusCallback() {

					@Override
					public void onAutoFocus(boolean success, Camera camera) {
						// 从Camera捕获图片
						mCamera.takePicture(null, null, mPicture);
					}
				});
			}
		});

		Button flashButton = (Button) findViewById(R.id.camera_flash);
		flashButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub

			}
		});
		Button changeButton = (Button) findViewById(R.id.camera_change);
		changeButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub

			}
		});
	}

	/** 检测设备是否存在Camera硬件 */
	private boolean checkCameraHardware(Context context) {
		if (context.getPackageManager().hasSystemFeature(
				PackageManager.FEATURE_CAMERA)) {
			// 存在
			return true;
		} else {
			// 不存在
			return false;
		}
	}

	/** 打开一个Camera */
	public static Camera getCameraInstance() {
		Camera c = null;
		try {
			c = Camera.open();
		} catch (Exception e) {
			Log.d(TAG, "打开Camera失败失败");
		}
		return c;
	}

	private PictureCallback mPicture = new PictureCallback() {

		@Override
		public void onPictureTaken(byte[] data, Camera camera) {
			Bitmap b = null;
			if (null != data) {
				b = BitmapFactory.decodeByteArray(data, 0, data.length);// data是字节数据，将其解析成位图
			}
			if (null != b) {
				Bitmap rotaBitmap = getRotateBitmap(b, 90.0f);
				saveBitmap(rotaBitmap);
			}

			// // 获取Jpeg图片，并保存在sd卡上
			// File pictureFile = new File("/sdcard/" +
			// System.currentTimeMillis()
			// + ".jpg");
			// try {
			// FileOutputStream fos = new FileOutputStream(pictureFile);
			// fos.write(data);
			// fos.close();
			// } catch (Exception e) {
			// Log.d(TAG, "保存图片失败");
			// }
		}
	};

	public static void saveBitmap(Bitmap b) {

		long dataTake = System.currentTimeMillis();
		String jpegName = "/sdcard/" + dataTake + ".jpg";
		Log.i(TAG, "saveBitmap:jpegName = " + jpegName);
		try {
			FileOutputStream fout = new FileOutputStream(jpegName);
			BufferedOutputStream bos = new BufferedOutputStream(fout);
			b.compress(Bitmap.CompressFormat.JPEG, 100, bos);
			bos.flush();
			bos.close();
			Log.i(TAG, "saveBitmap成功");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Log.i(TAG, "saveBitmap:失败");
			e.printStackTrace();
		}

	}

	@Override
	protected void onDestroy() {
		// 回收Camera资源
		if (mCamera != null) {
			mCamera.stopPreview();
			mCamera.release();
			mCamera = null;
		}
		super.onDestroy();
	}

	/**
	 * 图片旋转
	 * 
	 * @param b
	 * @param rotateDegree
	 * @return
	 */
	public static Bitmap getRotateBitmap(Bitmap b, float rotateDegree) {
		Matrix matrix = new Matrix();
		matrix.postRotate((float) rotateDegree);
		Bitmap rotaBitmap = Bitmap.createBitmap(b, 0, 0, b.getWidth(),
				b.getHeight(), matrix, false);
		return rotaBitmap;
	}
}