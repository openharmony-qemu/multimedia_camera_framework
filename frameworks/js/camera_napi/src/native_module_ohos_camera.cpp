/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "native_module_ohos_camera.h"

#include "input/camera_input_napi.h"
#include "input/camera_manager_napi.h"
#include "input/camera_napi.h"
#include "mode/photo_session_napi.h"
#include "mode/secure_camera_session_napi.h"
#include "mode/video_session_napi.h"
#include "output/capture_photo_napi.h"
#include "output/quick_thumbnail_napi.h"
#include "output/photo_napi.h"
#include "output/photo_output_napi.h"
#include "output/preview_output_napi.h"
#include "output/camera_output_capability.h"
#include "output/unify_movie_file_output_napi.h"
#include "output/video_output_napi.h"

namespace OHOS {
namespace CameraStandard {
static napi_status RegisterDepthEnums(napi_env env, napi_value exports)
{
    struct Member { const char *name; int32_t value; };
    static const Member accuracy[] = {
        {"DEPTH_DATA_ACCURACY_RELATIVE", DEPTH_DATA_ACCURACY_RELATIVE},
        {"DEPTH_DATA_ACCURACY_ABSOLUTE", DEPTH_DATA_ACCURACY_ABSOLUTE},
    };
    // Public quality-level wire values; actual depth metadata remains unchanged.
    static const Member quality[] = {
        {"DEPTH_DATA_QUALITY_BAD", 0}, {"DEPTH_DATA_QUALITY_FAIR", 1}, {"DEPTH_DATA_QUALITY_GOOD", 2},
    };
    struct Enum { const char *name; const Member *members; size_t count; };
    static const Enum enums[] = {
        {"DepthDataAccuracy", accuracy, sizeof(accuracy) / sizeof(accuracy[0])},
        {"DepthDataQualityLevel", quality, sizeof(quality) / sizeof(quality[0])},
    };
    for (const auto &entry : enums) {
        napi_value object = nullptr;
        napi_status status = napi_create_object(env, &object);
        if (status != napi_ok) {
            return status;
        }
        for (size_t index = 0; index < entry.count; ++index) {
            napi_value value = nullptr;
            status = napi_create_int32(env, entry.members[index].value, &value);
            if (status != napi_ok) {
                return status;
            }
            status = napi_set_named_property(env, object, entry.members[index].name, value);
            if (status != napi_ok) {
                return status;
            }
        }
        status = napi_set_named_property(env, exports, entry.name, object);
        if (status != napi_ok) {
            return status;
        }
    }
    return napi_ok;
}

/*
 * Function registering all props and functions of ohos.camera module
 */
static napi_value Export(napi_env env, napi_value exports)
{
    if (RegisterDepthEnums(env, exports) != napi_ok) {
        return nullptr;
    }
    MEDIA_DEBUG_LOG("Export called()");
    CameraInputNapi::Init(env, exports);
    PreviewOutputNapi::Init(env, exports);
    PhotoOutputNapi::Init(env, exports);
    QuickThumbnailNapi::Init(env, exports);
    VideoOutputNapi::Init(env, exports);
    CameraManagerNapi::Init(env, exports);
    CameraNapi::Init(env, exports);
    MetadataOutputNapi::Init(env, exports);
    PhotoSessionNapi::Init(env, exports);
    UnifyMovieFileOutputNapi::Init(env, exports);
    VideoSessionNapi::Init(env, exports);
    PhotoNapi::Init(env, exports);
    CapturePhotoNapi::Init(env, exports);
    SecureCameraSessionNapi::Init(env, exports);
    // Public caller-owned configuration record, not a camera service object.
    // Leave fields unset until assigned by the caller, as in the SDK declaration.
    napi_value effectSuggestionStatus = nullptr;
    auto constructor = [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value target = nullptr;
        if (napi_get_new_target(env, info, &target) != napi_ok) {
            return nullptr;
        }
        if (target == nullptr) {
            napi_throw_type_error(env, nullptr, "EffectSuggestionStatus requires new");
            return nullptr;
        }
        napi_value instance = nullptr;
        if (napi_get_cb_info(env, info, nullptr, nullptr, &instance, nullptr) != napi_ok) {
            return nullptr;
        }
        return instance;
    };
    if (napi_define_class(env, "EffectSuggestionStatus", NAPI_AUTO_LENGTH, constructor,
            nullptr, 0, nullptr, &effectSuggestionStatus) != napi_ok ||
        napi_set_named_property(env, exports, "EffectSuggestionStatus", effectSuggestionStatus) != napi_ok) {
        return nullptr;
    }
    MEDIA_DEBUG_LOG("Export is end");
    return exports;
}

/*
 * module define
 */
static napi_module g_module = { .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Export,
    .nm_modname = "multimedia.camera",
    .nm_priv = (reinterpret_cast<void*>(0)),
    .reserved = { 0 } };

/*
 * module register
 */
extern "C" __attribute__((constructor)) void RegisterCameraModule(void)
{
    MEDIA_DEBUG_LOG("RegisterCameraModule is called");
    napi_module_register(&g_module);
}
} // namespace CameraStandard
} // namespace OHOS
