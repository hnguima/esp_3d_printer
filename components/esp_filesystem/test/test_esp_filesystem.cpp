#include <iostream>
#include "unity.h"
#include "esp_filesystem.hpp"

void setUp(void)
{
  FileSystem::unmount_all();
}
void tearDown(void)
{
  FileSystem::unmount_all();
  FileSystem::mount("/test_data");

  if (FileSystem::is_file("/test_data/testfile.txt"))
  {
    FileSystem::remove("/test_data/testfile.txt");
  }
  if (FileSystem::is_file("/test_data/testdir/testfile.txt"))
  {
    FileSystem::remove("/test_data/testdir/testfile.txt");
  }
  if (FileSystem::is_dir("/test_data/testdir"))
  {
    FileSystem::remove("/test_data/testdir");
  }
  if (FileSystem::is_file("/test_data/new_testfile.txt"))
  {
    FileSystem::remove("/test_data/new_testfile.txt");
  }

  FileSystem::unmount_all();
}

TEST_CASE("Mount partition with name", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Mount all available data partitions", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount_all());
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount_all());
}

TEST_CASE("Mount a non existing partition", "[filesystem]")
{
  TEST_ASSERT_NOT_EQUAL(ESP_OK, FileSystem::mount("/non_existing_partition"));
}

TEST_CASE("Mount a partition that's already mounted", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data")); // should fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Mount a partition that's already mounted and remove a non-existing file", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data")); // should fail
  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::remove("/test_data/non_existing_file.txt"));
  
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create and delete file", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(false, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create and delete directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  FileSystem::remove("/test_data/testdir");
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create file inside new dir", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create file inside non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::create("/test_data/testdir/testfile.txt")); // should fail
  TEST_ASSERT_EQUAL(false, FileSystem::is_file("/test_data/testdir/testfile.txt"));   // should fail

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}
TEST_CASE("Create file that already exists", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create directory that already exists", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove non existing file", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove not empty directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove not empty directory with force", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force create file into a non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Move file to a new directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(false, FileSystem::is_file("/test_data/testfile.txt")); // has to fail

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Move file to a non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(false, FileSystem::is_file("/test_data/testdir/testfile.txt"));                            // has to fail
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                     // has to succeed

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Move file to a directory that already have a file with the same name", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_FAIL, FileSystem::move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));                             // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                     // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force move file  to a non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));                                 // has to succeed
  TEST_ASSERT_EQUAL(false, FileSystem::is_file("/test_data/testfile.txt"));                                        // has to fail'

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::copy("/test_data/testfile.txt", "/test_data/new_testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));     // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/new_testfile.txt")); // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/new_testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file with the same name", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, FileSystem::copy("/test_data/testfile.txt", "/test_data/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                      // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file to another directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                   // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));                           // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file to a non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, FileSystem::copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                     // has to succeed
  TEST_ASSERT_EQUAL(false, FileSystem::is_file("/test_data/testdir/testfile.txt"));                            // has to fail

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force copy file to a non existing directory", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                         // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));                                 // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force copy file with the same name", "[filesystem]")
{
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to succeed
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));                                         // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}