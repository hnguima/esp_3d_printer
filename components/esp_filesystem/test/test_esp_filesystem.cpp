#include "unity.h"
#include "esp_filesystem.hpp"

TEST_CASE("Mount partition with name", "[filesystem]")
{
  FileSystem::unmount_all();
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
  FileSystem::unmount_all();
  TEST_ASSERT_NOT_EQUAL(ESP_OK, FileSystem::mount("/non_existing_partition"));
}

TEST_CASE("Create and delete file", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_file("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create and delete directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  FileSystem::remove("/test_data/testfile");
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create file inside new dir", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create file inside non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));  // should fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt")); // should fail

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}
TEST_CASE("Create file that already exists", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Create directory that already exists", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove non existing file", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove not empty directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Remove not empty directory with force", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force create file into a non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Move file to a new directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt")); // has to fail

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Move file to a non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));              // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                      // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Move file to a directory that already have a file with the same name", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));              // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                      // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force move file  to a non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_move("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));                    // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                            // has to fail'

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::copy("/test_data/testfile.txt", "/test_data/new_testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));     // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/new_testfile.txt")); // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/new_testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file with the same name", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::copy("/test_data/testfile.txt", "/test_data/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));              // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file to another directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::make_dir("/test_data/testdir"));
  TEST_ASSERT_EQUAL(true, FileSystem::is_dir("/test_data/testdir"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                      // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));              // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Copy file to a non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                      // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));              // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force copy file to a non existing directory", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to fail
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                            // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));                    // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}

TEST_CASE("Force copy file with the same name", "[filesystem]")
{
  FileSystem::unmount_all();
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::mount("/test_data"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::create("/test_data/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_create("/test_data/testdir/testfile.txt"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testdir/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_copy("/test_data/testfile.txt", "/test_data/testdir/testfile.txt")); // has to succeed
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::is_file("/test_data/testfile.txt"));                            // has to succeed

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::force_remove("/test_data/testdir"));
  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::remove("/test_data/testfile.txt"));

  TEST_ASSERT_EQUAL(ESP_OK, FileSystem::unmount("/test_data"));
}