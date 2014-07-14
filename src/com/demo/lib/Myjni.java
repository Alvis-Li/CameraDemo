package com.demo.lib;

public class Myjni {

	// static {
	// System.loadLibrary("jni");
	// }

	public native void add(int i);

	public native void add2(String s1, String s2, String s3, String s4,
			int point1x, int point1y, int point2x, int point2y, int point3x,
			int point3y);
}
