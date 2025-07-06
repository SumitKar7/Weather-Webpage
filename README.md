
Welcome to my very own mini-filesystem project, coded in C! This project simulates a simple flat-file, CP/M/Unix-inspired file system inside a single disk file (like `disk01`). It supports commands to create, format, list, and manipulate simple directory and file entries, with user permissions baked in.  

**Key Features:**
- Custom file metadata (permissions, user, timestamps)
- Subdirectories (flat-level, no nested yet)
- File creation / removal
- Permissions modification (chmod style)
- Saving/loading from disk images
- Designed to be simple enough to learn from, but powerful enough to grow

---
To compile the program
 gcc fs.c -o fs

# 1. Create filesystem with 20 blocks
./fs createfs 20

# 2. Format it with 12 FNT entries and 4 DABPT entries
./fs formatfs 12 4

# 3. Save to disk image named disk1
./fs savefs disk1

# 4. List contents (should be empty)
./fs list

# 5. Show disk1 in hex (optional)
hexdump -v disk1

# 6. Add some files
./fs put f1
./fs put f1023

# 7. List contents
./fs list

# 8. Show disk again
hexdump -v disk1

# 9. Add more files
./fs put f255

# 10. Show disk contents
hexdump -v disk1

# 11. Attempt to remove a file that might not exist
./fs remove f312

# 12. Add another file
./fs put f2

# 13. Show disk again
hexdump -v disk1

# 14. Add another file
./fs put f312

# 15. Remove a file
./fs remove f2

# 16. List final state
./fs list

# 17. Show disk for final state
hexdump -v disk1


# 18. To rename the file
./fs rename f1 file1

# 19. To put external file
./fs putfile notes.txt
//To get it out again
./fs getfile notes.txt


# 20. Set user
./fs setuser {your name}

# 21. To link file
./fs link file1 sumit1

# 22. To unlink file
./fs unlink sumit1



# For the bonus part
./fs chmod f1 rwxr--r-- (myfile rwxr--r--)      # set permissions + enforce them
./fs mkdir f6  (/subdir/another)       # nested directory
./fs put f6/f5 (/subdir/another/file)    # create file in nested directory

#To create a file at the beginning
echo "This is file 1" > f1
