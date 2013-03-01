/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <pthread.h>
#include <hardware/hardware.h>
#include <hardware/camera3.h>
#include "Stream.h"

namespace default_camera_hal {
// Camera represents a physical camera on a device.
// This is constructed when the HAL module is loaded, one per physical camera.
// It is opened by the framework, and must be closed before it can be opened
// again.
class Camera {
    public:
        // id is used to distinguish cameras. 0 <= id < NUM_CAMERAS.
        // module is a handle to the HAL module, used when the device is opened.
        Camera(int id);
        ~Camera();

        // Common Camera Device Operations (see <hardware/camera_common.h>)
        int open(const hw_module_t *module, hw_device_t **device);
        int close();

        // Camera v3 Device Operations (see <hardware/camera3.h>)
        int initialize(const camera3_callback_ops_t *callback_ops);
        int configureStreams(camera3_stream_configuration_t *stream_list);
        int registerStreamBuffers(const camera3_stream_buffer_set_t *buf_set);
        const camera_metadata_t *constructDefaultRequestSettings(int type);
        int processCaptureRequest(camera3_capture_request_t *request);
        void getMetadataVendorTagOps(vendor_tag_query_ops_t *ops);
        void dump(int fd);

        // Camera device handle returned to framework for use
        camera3_device_t mDevice;

    private:
        // Reuse a stream already created by this device
        Stream *reuseStream(camera3_stream_t *astream);
        // Destroy all streams in a stream array, and the array itself
        void destroyStreams(Stream **array, int count);
        // Verify a set of streams is valid in aggregate
        bool isValidStreamSet(Stream **array, int count);
        // Calculate usage and max_bufs of each stream
        void setupStreams(Stream **array, int count);

        // Identifier used by framework to distinguish cameras
        const int mId;
        // Busy flag indicates camera is in use
        bool mBusy;
        // Camera device operations handle shared by all devices
        const static camera3_device_ops_t sOps;
        // Methods used to call back into the framework
        const camera3_callback_ops_t *mCallbackOps;
        // Lock protecting the Camera object for modifications
        pthread_mutex_t mMutex;
        // Array of handles to streams currently in use by the device
        Stream **mStreams;
        // Number of streams in mStreams
        int mNumStreams;
};
} // namespace default_camera_hal

#endif // CAMERA_H_
