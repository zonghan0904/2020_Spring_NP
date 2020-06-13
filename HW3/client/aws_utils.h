# ifndef __AWS_UTILS_H__
# define __AWS_UTILS_H__

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/Object.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

bool create_bucket(const Aws::String &bucket_name);
void delete_object(const Aws::String &bucket_name, const Aws::String &key_name);
void list_object(const Aws::String &bucket_name);
void get_object(const Aws::String &bucket_name, const Aws::String &key_name);
bool put_object(const Aws::String &bucket_name, const Aws::String &key_name, const std::string &content);

#endif
