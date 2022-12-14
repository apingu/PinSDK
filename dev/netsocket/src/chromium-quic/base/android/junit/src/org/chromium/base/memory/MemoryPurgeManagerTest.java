// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.base.memory;

import android.os.Looper;

import androidx.test.filters.SmallTest;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.annotation.Config;
import org.robolectric.shadows.ShadowLooper;

import org.chromium.base.ApplicationState;
import org.chromium.base.BaseFeatures;
import org.chromium.base.FakeTimeTestRule;
import org.chromium.base.FeatureList;
import org.chromium.base.ThreadUtils;
import org.chromium.base.test.BaseRobolectricTestRunner;

import java.util.Map;
import java.util.concurrent.TimeUnit;

/**
 * Tests for MemoryPurgeManager.
 */
@RunWith(BaseRobolectricTestRunner.class)
@Config(manifest = Config.NONE)
public class MemoryPurgeManagerTest {
    @Rule
    public FakeTimeTestRule mFakeTimeTestRule = new FakeTimeTestRule();

    private class MemoryPurgeManagerForTest extends MemoryPurgeManager {
        public MemoryPurgeManagerForTest(int initialState) {
            super();
            mApplicationState = initialState;
        }

        @Override
        public void onApplicationStateChange(int state) {
            mApplicationState = state;
            super.onApplicationStateChange(state);
        }

        @Override
        protected void notifyMemoryPressure() {
            mMemoryPressureNotifiedCount += 1;
        }

        @Override
        protected int getApplicationState() {
            return mApplicationState;
        }

        public int mMemoryPressureNotifiedCount;
        public int mApplicationState = ApplicationState.UNKNOWN;
    }

    @Before
    public void setUp() {
        // Explicitly set main thread as UiThread. Other places rely on that.
        ThreadUtils.setUiThread(Looper.getMainLooper());

        // Pause main thread to get control over when tasks are run (see runUiThreadFor()).
        ShadowLooper.pauseMainLooper();
    }

    @Test
    @SmallTest
    public void testSimple() {
        FeatureList.setTestFeatures(Map.of(BaseFeatures.BROWSER_PROCESS_MEMORY_PURGE, true));

        var manager = new MemoryPurgeManagerForTest(ApplicationState.HAS_RUNNING_ACTIVITIES);
        manager.start();

        // No notification when initial state has activities.
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);

        // Notify after a delay.
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount); // Should be delayed.
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);

        // Only one notification.
        manager.onApplicationStateChange(ApplicationState.HAS_DESTROYED_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);
    }

    @Test
    @SmallTest
    public void testInitializedOnceInBackground() {
        FeatureList.setTestFeatures(Map.of(BaseFeatures.BROWSER_PROCESS_MEMORY_PURGE, true));

        var manager = new MemoryPurgeManagerForTest(ApplicationState.HAS_STOPPED_ACTIVITIES);
        manager.start();
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);
    }

    @Test
    @SmallTest
    public void testDontTriggerForProcessesWithNoActivities() {
        FeatureList.setTestFeatures(Map.of(BaseFeatures.BROWSER_PROCESS_MEMORY_PURGE, true));

        var manager = new MemoryPurgeManagerForTest(ApplicationState.HAS_DESTROYED_ACTIVITIES);
        manager.start();

        // Don't purge if the process never hosted any activity.
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);

        // Starts when we cycle through foreground and background.
        manager.onApplicationStateChange(ApplicationState.HAS_RUNNING_ACTIVITIES);
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);
    }

    @Test
    @SmallTest
    public void testMultiple() {
        FeatureList.setTestFeatures(Map.of(BaseFeatures.BROWSER_PROCESS_MEMORY_PURGE, true));

        var manager = new MemoryPurgeManagerForTest(ApplicationState.HAS_RUNNING_ACTIVITIES);
        manager.start();

        // Notify after a delay.
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);

        // Back to foreground, no notification.
        manager.onApplicationStateChange(ApplicationState.HAS_RUNNING_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);

        // Background again, notify
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        Assert.assertEquals(2, manager.mMemoryPressureNotifiedCount);
    }

    @Test
    @SmallTest
    public void testNoEnoughTimeInBackground() {
        FeatureList.setTestFeatures(Map.of(BaseFeatures.BROWSER_PROCESS_MEMORY_PURGE, true));

        var manager = new MemoryPurgeManagerForTest(ApplicationState.HAS_RUNNING_ACTIVITIES);
        manager.start();

        // Background, then foregound inside the delay period.
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS / 2);
        manager.onApplicationStateChange(ApplicationState.HAS_RUNNING_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS);
        // Went back to foreground, do nothing.
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);

        // Starts the new task
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        // After some time, foregroung/background cycle.
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS / 2);
        manager.onApplicationStateChange(ApplicationState.HAS_RUNNING_ACTIVITIES);
        manager.onApplicationStateChange(ApplicationState.HAS_STOPPED_ACTIVITIES);
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS / 2);
        // Not enough time in background.
        Assert.assertEquals(0, manager.mMemoryPressureNotifiedCount);
        // But the task got rescheduled.
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS / 2);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);

        // No new notification.
        runUiThreadFor(MemoryPurgeManager.PURGE_DELAY_MS * 2);
        Assert.assertEquals(1, manager.mMemoryPressureNotifiedCount);
    }

    private void runUiThreadFor(long delayMs) {
        mFakeTimeTestRule.advanceMillis(delayMs);
        ShadowLooper.idleMainLooper(delayMs, TimeUnit.MILLISECONDS);
    }
}
