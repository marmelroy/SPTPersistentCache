#import <Foundation/Foundation.h>

#import "SPTPersistentDataStream.h"

FOUNDATION_EXPORT NSString *const SPTPersistentDataCacheErrorDomain;

/**
 * Default garbage collection interval. Some sane implementation defined value you should not care about.
 */
FOUNDATION_EXPORT const NSUInteger SPTPersistentDataCacheDefaultGCIntervalSec;

/**
 * Default exparation interval for all cache items. Particular record's TTL takes precedence over this value.
 * Items stored without (tt=0) TTL considered as expired if following is true: current_time - update_time > ExpInterval.
 */
FOUNDATION_EXPORT const NSUInteger SPTPersistentDataCacheDefaultExpirationTimeSec;


/**
 @enum SPTDataCacheResponseCode

 @discussion The SPTDataCacheResponseCode enum defines constants that
 is used to identify what kind of response would be given in callback to
 loadDataForKey:withCallback: method.

 @constant SPTDataCacheResponseCodeOperationSucceeded Indicates success of requested operation with data.
 record field of SPTPersistentCacheResponse mustn't be nil if it was load operation otherwise it could be. error would be nil.

 @constant SPTDataCacheResponseCodeNotFound Indicates that no file found for given key in cache or is expired.
 record and error field of SPTPersistentCacheResponse is nil in this case.

 @constant SPTDataCacheResponseCodeOperationError Indicates error occured during requested operation.
 record field of SPTPersistentCacheResponse would be nil. error mustn't be nil and specify exact error.
 */
typedef NS_ENUM(NSInteger, SPTDataCacheResponseCode) {
    SPTDataCacheResponseCodeOperationSucceeded,
    SPTDataCacheResponseCodeNotFound,
    SPTDataCacheResponseCodeOperationError
};


/**
 @enum SPTDataCacheLoadingError

 @discussion The SPTDataCacheLoadingError enum defines constants that
 identify NSError's in SPTPersistentDataCacheErrorDomain.

 @constant SPTDataCacheLoadingErrorMagicMismatch Magic number in record header is not as expected which means
 file is not readable by this cache.

 @constant SPTDataCacheLoadingErrorHeaderAlignmentMismatch Alignment of pointer which casted to header
 is not compatible with alignment of first header field. This actually is insane but who knows.

 @constant SPTDataCacheLoadingErrorWrongHeaderSize Size of header is not as expected. Possibly bcuz of version change.

 @constant SPTDataCacheLoadingErrorWrongPayloadSize Payload size in header is not the same as stored in cache record.

 @constant SPTDataCacheLoadingErrorInvalidHeaderCRC CRC calculated for reader and contained in header are different.

 @constant SPTDataCacheLoadingErrorNotEnoughDataToGetHeader Binary data size read as header is less then current header size
 which means we can't proceed further with this file.

 @constant SPTDataCacheLoadingErrorRecordIsStreamAndBusy Record is opened as stream and busy right now.

 @constant SPTDataCacheLoadingErrorInternalInconsistency Something bad has happened that shouldn't.
 */
typedef NS_ENUM(NSInteger, SPTDataCacheLoadingError) {
    SPTDataCacheLoadingErrorMagicMismatch = 100,
    SPTDataCacheLoadingErrorHeaderAlignmentMismatch,
    SPTDataCacheLoadingErrorWrongHeaderSize,
    SPTDataCacheLoadingErrorWrongPayloadSize,
    SPTDataCacheLoadingErrorInvalidHeaderCRC,
    SPTDataCacheLoadingErrorNotEnoughDataToGetHeader,
    SPTDataCacheLoadingErrorRecordIsStreamAndBusy,
    SPTDataCacheLoadingErrorInternalInconsistency
};


/**
 * @brief SPTDataCacheRecord
 *
 * @discussion Class defines one record in cache that is returned in response.
 *             Each record is represented by single file on disk.
 *             If file deleted from disk then cache assumes its never existed and return SPTDataCacheResponseCodeNotFound for load call.
 */
@interface SPTDataCacheRecord : NSObject
/*
 * Defines the number of times external logical references to this cache item. Initially is 0 if locked flag on store is NO.
 * Files with refCount > 0 is considered as locked by GC procedure. They also returned on load call regardless of expiration.
 */
@property (nonatomic, assign, readonly) NSUInteger refCount;
/**
 * Defines ttl for given record if applicable. 0 means not applicable.
 */
@property (nonatomic, assign, readonly) NSUInteger ttl;
/**
 * Key for that record.
 */
@property (nonatomic, strong, readonly) NSString *key;
/*
 * Data that was initially passed into storeData:...
 */
@property (nonatomic, strong, readonly) NSData *data;
@end


/**
 * @brief SPTPersistentCacheResponse
 *
 * @discussion Class defines one response passed in callback to call loadDataForKey:
 */
@interface SPTPersistentCacheResponse : NSObject
/**
 * @see SPTDataCacheResponseCode
 */
@property (nonatomic, assign, readonly) SPTDataCacheResponseCode result;
/**
 * Defines error of response if appliable
 */
@property (nonatomic, strong, readonly) NSError *error;
/**
 * @see SPTDataCacheRecord
 */
@property (nonatomic, strong, readonly) SPTDataCacheRecord *record;
@end

/**
 * Type of callback that is used  to provide a stream access for particular cache record.
 */
typedef void (^SPTDataCacheStreamCallback)(SPTDataCacheResponseCode result, id<SPTPersistentDataStream> stream, NSError *error);
