# ofiq_client.DefaultApi

All URIs are relative to */*

Method | HTTP request | Description
------------- | ------------- | -------------
[**face_image_quality**](DefaultApi.md#face_image_quality) | **POST** /face_image_quality | Returns face image quality analysis results including preprocessing results
[**health**](DefaultApi.md#health) | **GET** /health | Service Health Information
[**scalar_quality**](DefaultApi.md#scalar_quality) | **POST** /scalar_quality | Returns an overall image quality core
[**vector_quality**](DefaultApi.md#vector_quality) | **POST** /vector_quality | Returns quality scores for different face quality measurements

# **face_image_quality**
> AnalysisResponseData face_image_quality(body)

Returns face image quality analysis results including preprocessing results

Perform face image quality analysis, includes assessment results for face quality measurements and preprocessing results like face region bounding box, face landmarks and face region masks image quality analysis based on different face quality measurements. Returns an object that contains native and scalar scores for each image quality measurement.

### Example
```python
from __future__ import print_function
import time
import ofiq_client
from ofiq_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = ofiq_client.DefaultApi()
body = ofiq_client.AnalysisRequestData() # AnalysisRequestData | 

try:
    # Returns face image quality analysis results including preprocessing results
    api_response = api_instance.face_image_quality(body)
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->face_image_quality: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **body** | [**AnalysisRequestData**](AnalysisRequestData.md)|  | 

### Return type

[**AnalysisResponseData**](AnalysisResponseData.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **health**
> health()

Service Health Information

Returns the current state of the service.

### Example
```python
from __future__ import print_function
import time
import ofiq_client
from ofiq_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = ofiq_client.DefaultApi()

try:
    # Service Health Information
    api_instance.health()
except ApiException as e:
    print("Exception when calling DefaultApi->health: %s\n" % e)
```

### Parameters
This endpoint does not need any parameter.

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **scalar_quality**
> ScalarQualityResponseData scalar_quality(body)

Returns an overall image quality core

Performs an overall image quality analysis. Returns an object that contains the unified quality score for the face image provided.

### Example
```python
from __future__ import print_function
import time
import ofiq_client
from ofiq_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = ofiq_client.DefaultApi()
body = ofiq_client.AnalysisRequestData() # AnalysisRequestData | 

try:
    # Returns an overall image quality core
    api_response = api_instance.scalar_quality(body)
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->scalar_quality: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **body** | [**AnalysisRequestData**](AnalysisRequestData.md)|  | 

### Return type

[**ScalarQualityResponseData**](ScalarQualityResponseData.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **vector_quality**
> VectorQualityResponseData vector_quality(body)

Returns quality scores for different face quality measurements

Perform image quality analysis based on different face quality measurements. Returns an object that contains native and scalar scores for each image quality measurement.

### Example
```python
from __future__ import print_function
import time
import ofiq_client
from ofiq_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = ofiq_client.DefaultApi()
body = ofiq_client.AnalysisRequestData() # AnalysisRequestData | 

try:
    # Returns quality scores for different face quality measurements
    api_response = api_instance.vector_quality(body)
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->vector_quality: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **body** | [**AnalysisRequestData**](AnalysisRequestData.md)|  | 

### Return type

[**VectorQualityResponseData**](VectorQualityResponseData.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

