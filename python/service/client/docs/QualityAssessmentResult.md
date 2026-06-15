# QualityAssessmentResult

## Properties
Name | Type | Description | Notes
------------ | ------------- | ------------- | -------------
**measure** | **str** | Name of a face quality measurement, e.g. one of \\&#x27;UnifiedQualityScore\\&#x27;, \\&#x27;BackgroundUniformity\\&#x27;, \\&#x27;Sharpness\\&#x27; etc. | 
**native_score** | **float** | Score value as computed by the quality measure implementation | 
**scalar_score** | **int** | A scalar value from the interval [0,100]. Higher values mean higher quality. A value of -1.0 indicates a failed attempt to calculate a quality score or the value is unassigned | 

[[Back to Model list]](../README.md#documentation-for-models) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to README]](../README.md)

