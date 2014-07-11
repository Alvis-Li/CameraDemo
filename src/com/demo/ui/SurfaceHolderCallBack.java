package com.demo.ui;

import java.io.IOException;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.os.Build;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * ����һ��Ԥ����
 */
public class SurfaceHolderCallBack implements SurfaceHolder.Callback {
	private static final String TAG = "SurfaceHolderCallBack";
	private SurfaceHolder mHolder;
	private Camera mCamera;
	private Context context;

	public SurfaceHolderCallBack(SurfaceHolder mHolder, Camera mCamera,
			Context context) {
		super();
		this.mHolder = mHolder;
		this.mCamera = mCamera;
		this.context = context;
	}

	public void surfaceCreated(SurfaceHolder holder) {
		// ��Surface������֮�󣬿�ʼCamera��Ԥ��
		try {
			mCamera.setPreviewDisplay(holder);
			mCamera.startPreview();
		} catch (IOException e) {
			Log.d(TAG, "Ԥ��ʧ��");
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder) {

	}

	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		Log.e(TAG, "surfaceChanged");
		// Surface�����ı��ʱ�򽫱����ã���һ����ʾ�������ʱ��Ҳ�ᱻ����
		if (mHolder.getSurface() == null) {
			// ���SurfaceΪ�գ�����������
			return;
		}

		// ֹͣCamera��Ԥ��
		try {
			mCamera.stopPreview();
		} catch (Exception e) {
			Log.d(TAG, "��Surface�ı��ֹͣԤ������");
		}

		// ��Ԥ��ǰ����ָ��Camera�ĸ������

		// ���¿�ʼԤ��
		try {
			Camera.Parameters cameraParams = mCamera.getParameters();
			cameraParams.setPictureFormat(PixelFormat.JPEG);
			boolean portrait = isPortrait();
			configureCameraParameters(cameraParams, portrait);

			mCamera.setPreviewDisplay(mHolder);
			mCamera.startPreview();

		} catch (Exception e) {
			Log.d(TAG, "Ԥ��Camera����");
		}
	}

	public boolean isPortrait() {
		return (context.getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT);
	}

	private static final String CAMERA_PARAM_ORIENTATION = "orientation";
	private static final String CAMERA_PARAM_LANDSCAPE = "landscape";
	private static final String CAMERA_PARAM_PORTRAIT = "portrait";

	public void configureCameraParameters(Camera.Parameters cameraParams,
			boolean portrait) {
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.FROYO) { // for 2.1 and
																	// before
			if (portrait) {
				cameraParams.set(CAMERA_PARAM_ORIENTATION,
						CAMERA_PARAM_PORTRAIT);
			} else {
				cameraParams.set(CAMERA_PARAM_ORIENTATION,
						CAMERA_PARAM_LANDSCAPE);
			}
		} else { // for 2.2 and later
			int angle;
			Display display = ((Activity) context).getWindowManager()
					.getDefaultDisplay();
			switch (display.getRotation()) {
			case Surface.ROTATION_0: // This is display orientation
				angle = 90; // This is camera orientation
				break;
			case Surface.ROTATION_90:
				angle = 0;
				break;
			case Surface.ROTATION_180:
				angle = 270;
				break;
			case Surface.ROTATION_270:
				angle = 180;
				break;
			default:
				angle = 90;
				break;
			}
			mCamera.setDisplayOrientation(angle);
		}

		// CamParaUtil.getInstance().printSupportPictureSize(cameraParams);
		// CamParaUtil.getInstance().printSupportPreviewSize(cameraParams);
		// // ����PreviewSize��PictureSize
		// float previewRate = getScreenRate(context);
		// Size pictureSize = CamParaUtil.getInstance().getPropPictureSize(
		// cameraParams.getSupportedPictureSizes(), previewRate, 800);
		// cameraParams.setPictureSize(pictureSize.width, pictureSize.height);
		// Size previewSize = CamParaUtil.getInstance().getPropPreviewSize(
		// cameraParams.getSupportedPreviewSizes(), previewRate, 800);
		// cameraParams.setPreviewSize(previewSize.width, previewSize.height);
		// Log.e(TAG, "camera.width " + cameraParams.getPictureSize().width
		// + "  height " + cameraParams.getPictureSize().height);
		// Log.e(TAG, "previewSize.width " + previewSize.width + "  height "
		// + previewSize.height);
		// Log.e(TAG, "pictureSize.width " + pictureSize.width + "  height "
		// + pictureSize.height);
		int width = context.getResources().getDisplayMetrics().widthPixels;
		int height = width * 480 / 320;
		cameraParams.setPreviewSize(width, height);
		cameraParams.setPictureSize(320, 480);
		mCamera.setParameters(cameraParams);
	}

	public static Point getScreenMetrics(Context context) {
		DisplayMetrics dm = context.getResources().getDisplayMetrics();
		int w_screen = dm.widthPixels;
		int h_screen = dm.heightPixels;
		Log.i(TAG, "Screen---Width = " + w_screen + " Height = " + h_screen
				+ " densityDpi = " + dm.densityDpi);
		return new Point(w_screen, h_screen);

	}

	/**
	 * ��ȡ��Ļ�����
	 * 
	 * @param context
	 * @return
	 */
	public static float getScreenRate(Context context) {
		Point P = getScreenMetrics(context);
		float H = P.y;
		float W = P.x;
		return (H / W);
	}
}