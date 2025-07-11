/* Copyright 2024 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/pjrt/plugin/example_plugin/myplugin_c_pjrt_internal.h"

#include <cstdio>
#include <memory>
#include <utility>

#include "absl/status/status.h"
#include "xla/pjrt/c/pjrt_c_api.h"
#include "xla/pjrt/c/pjrt_c_api_layouts_extension.h"
#include "xla/pjrt/c/pjrt_c_api_wrapper_impl.h"
#include "xla/pjrt/extensions/example/example_extension.h"
#include "xla/pjrt/extensions/example/example_extension_cpp.h"
#include "xla/pjrt/extensions/example/example_extension_private.h"
#include "xla/pjrt/pjrt_client.h"
#include "xla/pjrt/plugin/example_plugin/example_extension_impl.h"
#include "xla/pjrt/plugin/example_plugin/myplugin_cpp_pjrt.h"

namespace myplugin_pjrt {

// Create my C++ client. Called by the C API and is the glue between the C API
// and the C++ API.
PJRT_Error* PJRT_MypluginClient_Create(PJRT_Client_Create_Args* args) {
  std::unique_ptr<xla::PjRtClient> client = CreateMyPluginPjrtClient();
  args->client = pjrt::CreateWrapperClient(std::move(client));
  printf("Creating PJRT Client from myplugin_pjrt.cc\n");
  return nullptr;
}

PJRT_Error* PJRT_MypluginExecuteContext_Create(
    PJRT_ExecuteContext_Create_Args* args) {
  return new PJRT_Error{absl::UnimplementedError(
      "ExecuteContext not supported for MyPlugin execution.")};
}

PJRT_Error* PJRT_MypluginDeviceTopology_Create(
    PJRT_TopologyDescription_Create_Args* args) {
  return new PJRT_Error{absl::UnimplementedError(
      "Topology not supported for MyPlugin compilation.")};
}

PJRT_Error* PJRT_MyPlugin_CreateExampleExtensionCpp(
    PJRT_ExampleExtension_CreateExampleExtensionCpp_Args* args) {
  xla::ExampleExtensionCpp* example_extension_cpp =
      new xla::ExampleExtensionImpl("standard_prefix: ", "myplugin_prefix: ");
  PJRT_ExampleExtensionCpp* opaque_extension_cpp = new PJRT_ExampleExtensionCpp{
      example_extension_cpp,
  };
  args->extension_cpp = opaque_extension_cpp;
  return nullptr;
}

PJRT_Error* PJRT_MyPlugin_DestroyExampleExtensionCpp(
    PJRT_ExampleExtension_DestroyExampleExtensionCpp_Args* args) {
  delete args->extension_cpp->extension_cpp;
  delete args->extension_cpp;
  return nullptr;
}

const PJRT_Api* GetMyPluginPjrtApi() {
  printf("C++ Calling GetPjrtApi");
  static PJRT_Layouts_Extension layouts_extension =
      pjrt::CreateLayoutsExtension(nullptr);

  static PJRT_Example_Extension example_extension =
      pjrt::CreateExampleExtension(&layouts_extension.base,
                                   PJRT_MyPlugin_CreateExampleExtensionCpp,
                                   PJRT_MyPlugin_DestroyExampleExtensionCpp);

  static const PJRT_Api pjrt_api = pjrt::CreatePjrtApi(
      myplugin_pjrt::PJRT_MypluginClient_Create,
      myplugin_pjrt::PJRT_MypluginExecuteContext_Create,
      myplugin_pjrt::PJRT_MypluginDeviceTopology_Create,
      pjrt::PJRT_Plugin_Initialize_NoOp, &example_extension.base,
      pjrt::PJRT_Plugin_Attributes_Xla);

  printf("MyPlugin called GetPjrtApi\n");
  return &pjrt_api;
}

}  // namespace myplugin_pjrt
