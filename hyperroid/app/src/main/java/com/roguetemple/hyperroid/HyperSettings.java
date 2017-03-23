package com.roguetemple.hyperroid;

import android.os.Bundle;
import android.preference.PreferenceActivity;

public class HyperSettings extends PreferenceActivity {
@Override
public void onCreate(Bundle savedInstanceState) {
	super.onCreate(savedInstanceState);
	addPreferencesFromResource(R.xml.preferences);
}
}
