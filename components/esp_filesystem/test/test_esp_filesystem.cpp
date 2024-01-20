#include "unity.h"
#include "esp_filesystem.hpp"


TEST_CASE("Mount partition with name", "[filesystem]")
{
  Filesystem::mount("");
  Filesystem::unmount("");
}

TEST_CASE("Mount all available data partitions", "[filesystem]")
{
  Filesystem::mount_all();
  Filesystem::unmount_all();
}

TEST_CASE("Create and delete file", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");
  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Create and delete directory", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");
  Filesystem::remove("testfile");

  Filesystem::unmount("");
}

TEST_CASE("Create file inside new dir", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::create("testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");

  Filesystem::remove("testdir/testfile.txt");
  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Create file inside non existing directory", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::create("testdir/testfile.txt");  // should fail
  Filesystem::is_file("testdir/testfile.txt"); // should fail

  Filesystem::unmount("");
}
TEST_CASE("Create file that already exists", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Create directory that already exists", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Remove non-existing file", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Remove non-existing directory", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Remove not empty directory", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::create("testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");

  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Remove not empty directory with force", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::create("testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");

  Filesystem::force_remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Mount a non existing partition", "[filesystem][fails]")
{
  Filesystem::mount("");
}

TEST_CASE("Move file to a new directory", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::move("testfile.txt", "testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");
  Filesystem::is_file("testfile.txt"); //has to fail

  Filesystem::remove("testdir/testfile.txt");
  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Move file to a non existing directory", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::move("testfile.txt", "testdir/testfile.txt"); //has to fail
  Filesystem::is_file("testdir/testfile.txt"); // has to fail
  Filesystem::is_file("testfile.txt"); // has to succeed

  Filesystem::remove("testdir/testfile.txt");
  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Move file to a directory that already have a file with the same name", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::create("testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");

  Filesystem::move("testfile.txt", "testdir/testfile.txt"); // has to fail
  Filesystem::is_file("testdir/testfile.txt");              // has to succeed
  Filesystem::is_file("testfile.txt");                      // has to succeed

  Filesystem::remove("testdir/testfile.txt");
  Filesystem::remove("testfile.txt");
  Filesystem::remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Copy file", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::copy("testfile.txt", "new_testfile.txt");
  Filesystem::is_file("testfile.txt");              // has to succeed
  Filesystem::is_file("new_testfile.txt");          // has to succeed

  Filesystem::remove("testfile.txt");
  Filesystem::remove("new_testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Copy file with the same name", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::copy("testfile.txt", "testfile.txt"); // has to fail
  Filesystem::is_file("testfile.txt");     // has to succeed

  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Copy file to another directory", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::mkdir("testdir");
  Filesystem::is_dir("testdir");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::copy("testfile.txt", "testdir/testfile.txt"); // has to fail
  Filesystem::is_file("testfile.txt");                      // has to succeed
  Filesystem::is_file("testdir/testfile.txt");              // has to succeed

  Filesystem::force_remove("testdir");
  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Copy file to a non existing directory", "[filesystem][fails]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::copy("testfile.txt", "testdir/testfile.txt"); // has to fail
  Filesystem::is_file("testfile.txt");                      // has to succeed
  Filesystem::is_file("testdir/testfile.txt");              // has to succeed

  Filesystem::force_remove("testdir");
  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Force copy file to a non existing directory", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::force_copy("testfile.txt", "testdir/testfile.txt"); // has to fail
  Filesystem::is_file("testfile.txt");                      // has to succeed
  Filesystem::is_file("testdir/testfile.txt");              // has to succeed

  Filesystem::force_remove("testdir");
  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Force copy file with the same name", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::force_create("testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");

  Filesystem::force_copy("testfile.txt", "testdir/testfile.txt"); // has to succeed
  Filesystem::is_file("testfile.txt");                            // has to succeed

  Filesystem::force_remove("testdir");
  Filesystem::remove("testfile.txt");

  Filesystem::unmount("");
}

TEST_CASE("Force move file  to a non existing directory", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::create("testfile.txt");
  Filesystem::is_file("testfile.txt");

  Filesystem::force_move("testfile.txt", "testdir/testfile.txt"); // has to succeed
  Filesystem::is_file("testdir/testfile.txt");                    // has to succeed
  Filesystem::is_file("testfile.txt");                            // has to fail'

  Filesystem::force_remove("testdir");

  Filesystem::unmount("");
}

TEST_CASE("Force create file into a non existing directory", "[filesystem]")
{
  Filesystem::mount("");

  Filesystem::force_create("testdir/testfile.txt");
  Filesystem::is_file("testdir/testfile.txt");

  Filesystem::force_remove("testdir");

  Filesystem::unmount("");
}

// write string on file and read it
// write bitstream on file and read it
// write on non existing file
// write long string on file and read it in batches 
// copy, move or create a file on not mounted partition 
