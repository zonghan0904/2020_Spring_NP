# include "aws_utils.h"
//snippet-end:[s3.cpp.create_bucket.inc]

/**
 * Create an Amazon S3 bucket in a specified region
 */
// snippet-start:[s3.cpp.create_bucket.code]
bool create_bucket(const Aws::String &bucket_name){
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    // Set up the request
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket_name);

    // Is the region other than us-east-1 (N. Virginia)?
    // if (region != Aws::S3::Model::BucketLocationConstraint::us_east_1)
    // {
    //     // Specify the region as a location constraint
    //     Aws::S3::Model::CreateBucketConfiguration bucket_config;
    //     bucket_config.SetLocationConstraint(region);
    //     request.SetCreateBucketConfiguration(bucket_config);
    // }

    // Create the bucket
    Aws::S3::S3Client s3_client;
    auto outcome = s3_client.CreateBucket(request);
    if (!outcome.IsSuccess())
    {
        auto err = outcome.GetError();
        std::cout << "ERROR: CreateBucket: " <<
            err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
        return false;
    }
    return true;
}


void delete_object(const Aws::String &bucket_name, const Aws::String &key_name){
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        std::cout << "Deleting" << key_name << " from S3 bucket: " <<
            bucket_name << std::endl;

        // snippet-start:[s3.cpp.delete_object.code]
        Aws::S3::S3Client s3_client;

        Aws::S3::Model::DeleteObjectRequest object_request;
        object_request.WithBucket(bucket_name).WithKey(key_name);

        auto delete_object_outcome = s3_client.DeleteObject(object_request);

        if (delete_object_outcome.IsSuccess())
        {
            std::cout << "Done!" << std::endl;
        }
        else
        {
            std::cout << "DeleteObject error: " <<
                delete_object_outcome.GetError().GetExceptionName() << " " <<
                delete_object_outcome.GetError().GetMessage() << std::endl;
        }
        // snippet-end:[s3.cpp.delete_object.code]
    }
    Aws::ShutdownAPI(options);
}


void list_object(const Aws::String &bucket_name){
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        std::cout << "Objects in S3 bucket: " << bucket_name << std::endl;

        // snippet-start:[s3.cpp.list_objects.code]
        Aws::S3::S3Client s3_client;

        Aws::S3::Model::ListObjectsRequest objects_request;
        objects_request.WithBucket(bucket_name);

        auto list_objects_outcome = s3_client.ListObjects(objects_request);

        if (list_objects_outcome.IsSuccess())
        {
            Aws::Vector<Aws::S3::Model::Object> object_list =
                list_objects_outcome.GetResult().GetContents();

            for (auto const &s3_object : object_list)
            {
                std::cout << "* " << s3_object.GetKey() << std::endl;
            }
        }
        else
        {
            std::cout << "ListObjects error: " <<
                list_objects_outcome.GetError().GetExceptionName() << " " <<
                list_objects_outcome.GetError().GetMessage() << std::endl;
        }
        // snippet-end:[s3.cpp.list_objects.code]
    }

    Aws::ShutdownAPI(options);
}


void get_object(const Aws::String &bucket_name, const Aws::String &key_name){
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        // Set up the request
        Aws::S3::S3Client s3_client;
        Aws::S3::Model::GetObjectRequest object_request;
        object_request.SetBucket(bucket_name);
        object_request.SetKey(key_name);

        // Get the object
        auto get_object_outcome = s3_client.GetObject(object_request);
        if (get_object_outcome.IsSuccess())
        {
            // Get an Aws::IOStream reference to the retrieved file
            auto &retrieved_file = get_object_outcome.GetResultWithOwnership().GetBody();

#if 1
            // Output the first line of the retrieved text file
            std::cout << "Beginning of file contents:\n";
            char file_data[255] = { 0 };
            retrieved_file.getline(file_data, 254);
            std::cout << file_data << std::endl;
#else
            // Alternatively, read the object's contents and write to a file
            const char * filename = "/PATH/FILE_NAME";
            std::ofstream output_file(filename, std::ios::binary);
            output_file << retrieved_file.rdbuf();
#endif
        }
        else
        {
            auto error = get_object_outcome.GetError();
            std::cout << "ERROR: " << error.GetExceptionName() << ": "
                << error.GetMessage() << std::endl;
        }
        // snippet-end:[s3.cpp.get_object.code]
    }
    Aws::ShutdownAPI(options);
}


bool put_object(const Aws::String &bucket_name, const Aws::String &key_name, const std::string &content){
    Aws::Client::ClientConfiguration clientConfig;
    // Set up request
    Aws::S3::S3Client s3_client(clientConfig);
    Aws::S3::Model::PutObjectRequest object_request;

    object_request.SetBucket(bucket_name);
    object_request.SetKey(key_name);
    const std::shared_ptr<Aws::IOStream> input_data =
        Aws::MakeShared<Aws::StringStream>("");
    *input_data << content.c_str();
    object_request.SetBody(input_data);

    // Put the string into the S3 object
    auto put_object_outcome = s3_client.PutObject(object_request);
    if (!put_object_outcome.IsSuccess()) {
        auto error = put_object_outcome.GetError();
        std::cout << "ERROR: " << error.GetExceptionName() << ": "
            << error.GetMessage() << std::endl;
        return false;
    }
    return true;
}



