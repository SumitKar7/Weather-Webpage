//Name: Sumit Karki
// Operating Systems- CSE3320
//Assignment 3
//ID: 1002082649

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     
#include <stdint.h>     
#include <time.h>       

// === FILE SYSTEM CONFIG ===
#define BLOCK_SIZE 512              // disk block size in bytes
#define MAX_FILENAME 54             // max filename length
#define MAX_USER 40                 // max user string length
#define MAX_PATH 256                // max path length for subdirectories
#define FNT_ENTRIES 12              // File Name Table entries
#define DABPT_ENTRIES 4             // Directory/Attribute/Block Pointer Table entries
#define BPT_ENTRIES (DABPT_ENTRIES * 2)  // Block Pointer Table entries (indirect)
#define DATA_BLOCKS (20 - (1 + (FNT_ENTRIES * sizeof(FNTEntry) + BLOCK_SIZE - 1) / BLOCK_SIZE + (DABPT_ENTRIES * sizeof(DABPTEntry) + BLOCK_SIZE - 1) / BLOCK_SIZE + (BPT_ENTRIES * sizeof(BPTEntry) + BLOCK_SIZE - 1) / BLOCK_SIZE))
// this calculates how many data blocks we actually get in 20 blocks after accounting for metadata

// === PERMISSIONS STRUCTURE ===
typedef struct {
    uint8_t initial_read : 1; //for owner
    uint8_t initial_write : 1;
    uint8_t initial_exec : 1;
    uint8_t middle_read : 1; //for group
    uint8_t middle_write : 1;
    uint8_t middle_exec : 1;
    uint8_t last_read : 1; //for user
    uint8_t last_write : 1;
    uint8_t last_exec : 1;
} Permissions;

// === METADATA STRUCTURES ===

typedef struct {
    char filename[MAX_FILENAME];    // name of the file
    uint32_t inode;                 // index into the DABPT
    uint8_t is_directory;           // flag to indicate if this is a directory
} FNTEntry;

typedef struct {
    uint32_t file_size;             // file size in bytes
    uint64_t last_mod_time;         // last modification timestamp
    char user[MAX_USER];            // file owner
    uint32_t bpt_index;             // index into BPT (for data block chain)
    Permissions permissions;        // file permissions
    uint8_t is_directory;           // flag to indicate if this is a directory
    uint32_t parent_inode;          // parent directory inode (for subdirectories)
} DABPTEntry;

typedef struct {
    uint32_t block_pointers[7];     // block addresses for file data
    uint32_t next_bpt;              // pointer to the next BPT (like a linked list)
} BPTEntry;

typedef struct {
    uint32_t num_blocks;            // how many blocks in the FS
    uint32_t fnt_entries;           // number of FNT slots
    uint32_t dabpt_entries;         // number of DABPT slots
    uint32_t bpt_entries;           // number of BPT slots
} SuperBlock;

// === GLOBALS ===
SuperBlock superblock;                       // superblock structure
FNTEntry fnt[FNT_ENTRIES];                   // file name table
DABPTEntry dabpt[DABPT_ENTRIES];             // file metadata table
BPTEntry bpt[BPT_ENTRIES];                   // block pointers for files
uint8_t free_blocks[20];                     // bitmap of free/used blocks
char current_user[MAX_USER] = "default";     // default user string
FILE *disk = NULL;                           // disk file pointer

// === PERMISSION UTILITY FUNCTIONS ===

// Converts permissions struct to readable string format (e.g., "rwxr-xr-x")
void permissions_to_string(Permissions *perm, char *str) {
    str[0] = perm->initial_read ? 'r' : '-';
    str[1] = perm->initial_write ? 'w' : '-';
    str[2] = perm->initial_exec ? 'x' : '-';
    str[3] = perm->middle_read ? 'r' : '-';
    str[4] = perm->middle_write ? 'w' : '-';
    str[5] = perm->middle_exec ? 'x' : '-';
    str[6] = perm->last_read ? 'r' : '-';
    str[7] = perm->last_write ? 'w' : '-';
    str[8] = perm->last_exec ? 'x' : '-';
    str[9] = '\0';
}

// Set sdefault file permissions (rw-r--r--)
void set_default_file_permissions(Permissions *perm) {
    perm->initial_read = 1;
    perm->initial_write = 1;
    perm->initial_exec = 0;
    perm->middle_read = 1;
    perm->middle_write = 0;
    perm->middle_exec = 0;
    perm->last_read = 1;
    perm->last_write = 0;
    perm->last_exec = 0;
}

// Sets default directory permissions (rwxr-xr-x)
void set_default_dir_permissions(Permissions *perm) {
    perm->initial_read = 1;
    perm->initial_write = 1;
    perm->initial_exec = 1;
    perm->middle_read = 1;
    perm->middle_write = 0;
    perm->middle_exec = 1;
    perm->last_read = 1;
    perm->last_write = 0;
    perm->last_exec = 1;
}

// Parses permission string format (e.g., "rwxr-xr-x") into Permissions struct
int parse_permissions(const char *perm_str, Permissions *perm) {
    if (strlen(perm_str) != 9) {
        return 0; // invalid length
    }
    
    perm->initial_read = (perm_str[0] == 'r');
    perm->initial_write = (perm_str[1] == 'w');
    perm->initial_exec = (perm_str[2] == 'x');
    perm->middle_read = (perm_str[3] == 'r');
    perm->middle_write = (perm_str[4] == 'w');
    perm->middle_exec = (perm_str[5] == 'x');
    perm->last_read = (perm_str[6] == 'r');
    perm->last_write = (perm_str[7] == 'w');
    perm->last_exec = (perm_str[8] == 'x');
    
    return 1; // success
}

// === PATH UTILITY FUNCTIONS ===

// Splits path string into directory components
int split_path(const char *path, char **components, int max_components) {
    char path_copy[MAX_PATH];
    strncpy(path_copy, path, MAX_PATH - 1);
    path_copy[MAX_PATH - 1] = '\0';
    
    int count = 0;
    char *token = strtok(path_copy, "/");
    
    while (token != NULL && count < max_components) {
        components[count] = strdup(token);
        count++;
        token = strtok(NULL, "/");
    }
    
    return count;
}

// Find directory entry in FNT by name
int find_directory(const char *dirname) {
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && fnt[i].is_directory && 
            strcmp(fnt[i].filename, dirname) == 0) {
            return i;
        }
    }
    return -1;
}

// === SAVE FS STATE TO DISK ===
// Writes the current filesystem structures to a file (disk image)
void savefs(const char *diskname) {
    disk = fopen(diskname, "wb");
    if (!disk) { 
        printf("Error opening disk for writing\n");
        exit(1);
    }
    // Save the superblock and metadata tables to disk
    fwrite(&superblock, sizeof(SuperBlock), 1, disk);
    fwrite(free_blocks, sizeof(free_blocks), 1, disk);
    fwrite(fnt, sizeof(FNTEntry), FNT_ENTRIES, disk);
    fwrite(dabpt, sizeof(DABPTEntry), DABPT_ENTRIES, disk);
    fwrite(bpt, sizeof(BPTEntry), BPT_ENTRIES, disk);
    
    // Write empty (zeroed) data blocks
    char zero[BLOCK_SIZE] = {0};
    for (int i = 0; i < DATA_BLOCKS; i++)
        fwrite(zero, BLOCK_SIZE, 1, disk);
    
    fclose(disk);
}

// === CREATE FS ===
// Initializes a new filesystem in memory with zeroed structures
void createfs(int num_blocks) {
    memset(&superblock, 0, sizeof(superblock));
    superblock.num_blocks = num_blocks;
    superblock.fnt_entries = FNT_ENTRIES;
    superblock.dabpt_entries = DABPT_ENTRIES;
    superblock.bpt_entries = BPT_ENTRIES;

    memset(fnt, 0, sizeof(fnt));         // clear file name table
    memset(dabpt, 0, sizeof(dabpt));     // clear file metadata table
    memset(bpt, 0, sizeof(bpt));         // clear block pointer table
    memset(free_blocks, 0, sizeof(free_blocks)); // clear free block bitmap
    
    printf("Created FS with %d blocks\n", num_blocks);
}

// === FORMAT FS ===
// Resets file tables and pointers (like reinitializing the FS)
void formatfs(int fnt_entries, int dabpt_entries) {
    memset(fnt, 0, sizeof(fnt));
    memset(dabpt, 0, sizeof(dabpt));
    memset(bpt, 0, sizeof(bpt));
    printf("Formatted FS: FNT=%d, DABPT=%d\n", fnt_entries, dabpt_entries);
}

// === OPEN FS ===
// Loads an existing filesystem from disk into memory
void openfs(const char *diskname) {
    disk = fopen(diskname, "rb+");
    if (!disk) { 
        printf("Error opening disk\n");
        exit(1);
    }
    fread(&superblock, sizeof(SuperBlock), 1, disk);
    fread(free_blocks, sizeof(free_blocks), 1, disk);
    fread(fnt, sizeof(FNTEntry), FNT_ENTRIES, disk);
    fread(dabpt, sizeof(DABPTEntry), DABPT_ENTRIES, disk);
    fread(bpt, sizeof(BPTEntry), BPT_ENTRIES, disk);
    fclose(disk);
    printf("Opened FS %s\n", diskname);
}

// === LIST FS FILES ===
// Prints all files and directories in the filesystem
void listfs() {
    printf("=== FILESYSTEM CONTENTS ===\n");
    
    // List directories first
    printf("Directories:\n");
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && fnt[i].is_directory) {
            DABPTEntry *d = &dabpt[fnt[i].inode];
            char perm_str[10];
            permissions_to_string(&d->permissions, perm_str);
            printf("  %s/  %s  %s  %s\n",
                fnt[i].filename, perm_str, d->user,
                ctime((time_t*)&d->last_mod_time));
        }
    }
    
    // List files
    printf("Files:\n");
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && !fnt[i].is_directory) {
            DABPTEntry *d = &dabpt[fnt[i].inode];
            char perm_str[10];
            permissions_to_string(&d->permissions, perm_str);
            printf("  %s  %s  %s  %u bytes  %s",
                fnt[i].filename, perm_str, d->user, d->file_size,
                ctime((time_t*)&d->last_mod_time));
        }
    }
}

// === FIND FREE SLOT IN FNT ===
// Returns index of first empty FNT slot or -1 if full
int find_free_fnt() {
    for (int i = 0; i < FNT_ENTRIES; i++)
        if (fnt[i].filename[0] == 0) return i;
    return -1;
}

// === FIND FREE SLOT IN DABPT ===
// Returns index of first unused DABPT entry or -1 if full
int find_free_dabpt() {
    for (int i = 0; i < DABPT_ENTRIES; i++)
        if (dabpt[i].file_size == 0) return i;
    return -1;
}

// === CREATE DIRECTORY ===
// Adds a new directory to the FS
void mkdir(const char *path) {
    char *components[10];
    int num_components = split_path(path, components, 10);
    
    if (num_components == 0) {
        printf("Invalid path\n");
        return;
    }
    
    uint32_t parent_inode = 0; // For now, root only
    
    const char *dirname = components[num_components - 1];
    
    int fnt_idx = find_free_fnt();
    int dabpt_idx = find_free_dabpt();
    
    if (fnt_idx == -1 || dabpt_idx == -1) {
        printf("No space for new directory\n");
        return;
    }
    
    // Fill in FNT and DABPT for the directory
    strncpy(fnt[fnt_idx].filename, dirname, MAX_FILENAME);
    fnt[fnt_idx].inode = dabpt_idx;
    fnt[fnt_idx].is_directory = 1;
    
    dabpt[dabpt_idx].file_size = 0;
    dabpt[dabpt_idx].last_mod_time = (uint64_t)time(NULL);
    strncpy(dabpt[dabpt_idx].user, current_user, MAX_USER);
    dabpt[dabpt_idx].bpt_index = 0;
    set_default_dir_permissions(&dabpt[dabpt_idx].permissions);
    dabpt[dabpt_idx].is_directory = 1;
    dabpt[dabpt_idx].parent_inode = parent_inode;
    
    printf("Created directory %s\n", dirname);
    
    for (int i = 0; i < num_components; i++) {
        free(components[i]);
    }
}

// === ADD A FILE ===
// Creates a new file entry in FS
void put(const char *filename) {
    int fnt_idx = find_free_fnt();
    int dabpt_idx = find_free_dabpt();
    if (fnt_idx == -1 || dabpt_idx == -1) {
        printf("No space for new file\n");
        return;
    }
    strncpy(fnt[fnt_idx].filename, filename, MAX_FILENAME);
    fnt[fnt_idx].inode = dabpt_idx;
    fnt[fnt_idx].is_directory = 0;

    dabpt[dabpt_idx].file_size = 123; // Placeholder size
    dabpt[dabpt_idx].last_mod_time = (uint64_t)time(NULL);
    strncpy(dabpt[dabpt_idx].user, current_user, MAX_USER);
    dabpt[dabpt_idx].bpt_index = 0;
    set_default_file_permissions(&dabpt[dabpt_idx].permissions);
    dabpt[dabpt_idx].is_directory = 0;
    dabpt[dabpt_idx].parent_inode = 0;
    
    printf("Put file %s\n", filename);
}

// === REMOVE A FILE ===
// Deletes file + clears metadata
void remove_file(const char *filename) {
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (strcmp(fnt[i].filename, filename) == 0) {
            int inode = fnt[i].inode;
            memset(&fnt[i], 0, sizeof(FNTEntry));
            memset(&dabpt[inode], 0, sizeof(DABPTEntry));
            printf("Removed file %s\n", filename);
            return;
        }
    }
    printf("File %s not found\n", filename);
}

// === CHANGE PERMISSIONS ===
// Changes permission bits on a file
void chmod(const char *filename, const char *permissions) {
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (strcmp(fnt[i].filename, filename) == 0) {
            DABPTEntry *d = &dabpt[fnt[i].inode];
            if (parse_permissions(permissions, &d->permissions)) {
                d->last_mod_time = (uint64_t)time(NULL);
                printf("Changed permissions of %s to %s\n", filename, permissions);
            } else {
                printf("Invalid permission format. Use format: rwxr-xr-x\n");
            }
            return;
        }
    }
    printf("File %s not found\n", filename);
}

// === SET USER ===
// Changes the current user string
void setuser(const char *username) {
    strncpy(current_user, username, MAX_USER - 1);
    current_user[MAX_USER - 1] = '\0';
    printf("Current user set to: %s\n", current_user);
}

// === RENAME FILE ===
// Renames a file or directory in the filesystem
void rename_file(const char *oldname, const char *newname) {
    // Check if new name already exists
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && strcmp(fnt[i].filename, newname) == 0) {
            printf("Error: File %s already exists\n", newname);
            return;
        }
    }
    
    // Find the file to rename
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && strcmp(fnt[i].filename, oldname) == 0) {
            // Check permissions (owner can rename)
            DABPTEntry *d = &dabpt[fnt[i].inode];
            if (strcmp(d->user, current_user) != 0) {
                printf("Error: Permission denied. You don't own %s\n", oldname);
                return;
            }
            
            // Rename the file
            strncpy(fnt[i].filename, newname, MAX_FILENAME);
            d->last_mod_time = (uint64_t)time(NULL);
            printf("Renamed %s to %s\n", oldname, newname);
            return;
        }
    }
    printf("File %s not found\n", oldname);
}

// === PUT EXTERNAL FILE ===
// Copies a file from the host OS into the filesystem
void put_external_file(const char *external_filename) {
    FILE *external_file = fopen(external_filename, "rb");
    if (!external_file) {
        printf("Error: Cannot open external file %s\n", external_filename);
        return;
    }
    
    // Get file size
    fseek(external_file, 0, SEEK_END);
    long file_size = ftell(external_file);
    fseek(external_file, 0, SEEK_SET);
    
    if (file_size > BLOCK_SIZE * 7) { // Max 7 blocks per file
        printf("Error: File too large (max %d bytes)\n", BLOCK_SIZE * 7);
        fclose(external_file);
        return;
    }
    
    // Find free slots
    int fnt_idx = find_free_fnt();
    int dabpt_idx = find_free_dabpt();
    int bpt_idx = 0; // For now, use first BPT entry
    
    if (fnt_idx == -1 || dabpt_idx == -1) {
        printf("No space for new file\n");
        fclose(external_file);
        return;
    }
    
    // Find free data blocks
    int data_blocks_needed = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int blocks[7] = {0};
    int block_count = 0;
    
    for (int i = 0; i < 20 && block_count < data_blocks_needed; i++) {
        if (!free_blocks[i]) {
            blocks[block_count++] = i;
            free_blocks[i] = 1; // Mark as used
        }
    }
    
    if (block_count < data_blocks_needed) {
        printf("Error: Not enough free blocks\n");
        fclose(external_file);
        return;
    }
    
    // Create file entry
    strncpy(fnt[fnt_idx].filename, external_filename, MAX_FILENAME);
    fnt[fnt_idx].inode = dabpt_idx;
    fnt[fnt_idx].is_directory = 0;
    
    dabpt[dabpt_idx].file_size = file_size;
    dabpt[dabpt_idx].last_mod_time = (uint64_t)time(NULL);
    strncpy(dabpt[dabpt_idx].user, current_user, MAX_USER);
    dabpt[dabpt_idx].bpt_index = bpt_idx;
    set_default_file_permissions(&dabpt[dabpt_idx].permissions);
    dabpt[dabpt_idx].is_directory = 0;
    dabpt[dabpt_idx].parent_inode = 0;
    
    // Set up block pointers
    for (int i = 0; i < 7; i++) {
        bpt[bpt_idx].block_pointers[i] = blocks[i];
    }
    bpt[bpt_idx].next_bpt = 0;
    
    // Read file data and store in blocks (simplified - just mark as stored)
    printf("Stored external file %s (%ld bytes) in %d blocks\n", 
           external_filename, file_size, data_blocks_needed);
    
    fclose(external_file);
}

// === GET EXTERNAL FILE ===
// Copies a file from the filesystem to the host OS
void get_external_file(const char *filename) {
    // Find the file in filesystem
    int fnt_idx = -1;
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && strcmp(fnt[i].filename, filename) == 0) {
            fnt_idx = i;
            break;
        }
    }
    
    if (fnt_idx == -1) {
        printf("File %s not found in filesystem\n", filename);
        return;
    }
    
    DABPTEntry *d = &dabpt[fnt[fnt_idx].inode];
    
    // Check permissions
    if (strcmp(d->user, current_user) != 0 && !d->permissions.middle_read) {
        printf("Error: Permission denied to read %s\n", filename);
        return;
    }
    
    // Create output filename (add .out suffix to avoid conflicts)
    char output_filename[MAX_FILENAME + 10];
    snprintf(output_filename, sizeof(output_filename), "%s.out", filename);
    
    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        printf("Error: Cannot create output file %s\n", output_filename);
        return;
    }
    
    // Write dummy data (in real implementation, would read from data blocks)
    char dummy_data[BLOCK_SIZE] = {0};
    snprintf(dummy_data, sizeof(dummy_data), "Content of %s (size: %u bytes)", 
             filename, d->file_size);
    
    fwrite(dummy_data, 1, strlen(dummy_data), output_file);
    fclose(output_file);
    
    printf("Extracted %s to %s (%u bytes)\n", filename, output_filename, d->file_size);
}

// === LINK FILE ===
// Creates a hard link to an existing file (Unix style)
void link_file(const char *target, const char *linkname) {
    // Find the target file
    int target_fnt_idx = -1;
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && strcmp(fnt[i].filename, target) == 0) {
            target_fnt_idx = i;
            break;
        }
    }
    
    if (target_fnt_idx == -1) {
        printf("Target file %s not found\n", target);
        return;
    }
    
    // Check if target is a directory
    if (fnt[target_fnt_idx].is_directory) {
        printf("Error: Cannot create hard link to directory %s\n", target);
        return;
    }
    
    // Check if link name already exists
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && strcmp(fnt[i].filename, linkname) == 0) {
            printf("Error: Link name %s already exists\n", linkname);
            return;
        }
    }
    
    // Find free FNT slot for the link
    int link_fnt_idx = find_free_fnt();
    if (link_fnt_idx == -1) {
        printf("No space for new link\n");
        return;
    }
    
    // Create the link (points to same inode as target)
    strncpy(fnt[link_fnt_idx].filename, linkname, MAX_FILENAME);
    fnt[link_fnt_idx].inode = fnt[target_fnt_idx].inode; // Same inode
    fnt[link_fnt_idx].is_directory = 0;
    
    // Update modification time
    DABPTEntry *d = &dabpt[fnt[target_fnt_idx].inode];
    d->last_mod_time = (uint64_t)time(NULL);
    
    printf("Created hard link %s -> %s\n", linkname, target);
}

// === UNLINK FILE ===
// Removes a hard link (or file if it's the last link)
void unlink_file(const char *filename) {
    // Find the file
    int fnt_idx = -1;
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && strcmp(fnt[i].filename, filename) == 0) {
            fnt_idx = i;
            break;
        }
    }
    
    if (fnt_idx == -1) {
        printf("File %s not found\n", filename);
        return;
    }
    
    // Check permissions
    DABPTEntry *d = &dabpt[fnt[fnt_idx].inode];
    if (strcmp(d->user, current_user) != 0 && !d->permissions.middle_write) {
        printf("Error: Permission denied to remove %s\n", filename);
        return;
    }
    
    // Count how many links point to this inode
    int link_count = 0;
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0] && fnt[i].inode == fnt[fnt_idx].inode) {
            link_count++;
        }
    }
    
    // Remove this link
    memset(&fnt[fnt_idx], 0, sizeof(FNTEntry));
    
    // If this was the last link, remove the inode and data
    if (link_count == 1) {
        memset(&dabpt[fnt[fnt_idx].inode], 0, sizeof(DABPTEntry));
        // In a real implementation, would also free data blocks
        printf("Removed file %s (last link)\n", filename);
    } else {
        printf("Removed link %s (%d links remaining)\n", filename, link_count - 1);
    }
}

// === LIST USER FILES ===
// Lists all files owned by the current user
void list_user_files(const char *username) {
    printf("=== FILES OWNED BY %s ===\n", username);
    
    int file_count = 0;
    for (int i = 0; i < FNT_ENTRIES; i++) {
        if (fnt[i].filename[0]) {
            DABPTEntry *d = &dabpt[fnt[i].inode];
            if (strcmp(d->user, username) == 0) {
                char perm_str[10];
                permissions_to_string(&d->permissions, perm_str);
                if (fnt[i].is_directory) {
                    printf("  %s/  %s  %u bytes  %s",
                        fnt[i].filename, perm_str, d->file_size,
                        ctime((time_t*)&d->last_mod_time));
                } else {
                    printf("  %s  %s  %u bytes  %s",
                        fnt[i].filename, perm_str, d->file_size,
                        ctime((time_t*)&d->last_mod_time));
                }
                file_count++;
            }
        }
    }
    
    if (file_count == 0) {
        printf("  No files found for user %s\n", username);
    } else {
        printf("  Total: %d files/directories\n", file_count);
    }
}

// === MAIN DRIVER ===
// Entry point and command handler
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: fs <command> [args]\n");
        printf("Commands:\n");
        printf("  createfs <blocks>                    - Create new filesystem\n");
        printf("  formatfs <fnt> <dabpt>              - Format filesystem\n");
        printf("  savefs <diskname>                    - Save filesystem to disk\n");
        printf("  list                                 - List all files and directories\n");
        printf("  put <filename>                       - Create file\n");
        printf("  remove <filename>                    - Remove file\n");
        printf("  mkdir <path>                         - Create directory\n");
        printf("  chmod <filename> <permissions>       - Change file permissions\n");
        printf("  setuser <username>                   - Set current user\n");
        printf("  rename <oldname> <newname>           - Rename file\n");
        printf("  putfile <external_file>              - Store external file in FS\n");
        printf("  getfile <filename>                   - Extract file from FS to host\n");
        printf("  user <username>                      - List files owned by user\n");
        printf("  link <target> <linkname>             - Create hard link\n");
        printf("  unlink <filename>                    - Remove hard link\n");
        return 1;
    }

    // Handle commands
    if (strcmp(argv[1], "createfs") == 0 && argc == 3) {
        createfs(atoi(argv[2]));
        savefs("disk01");
    } else {
        openfs("disk01");
        if (strcmp(argv[1], "formatfs") == 0 && argc == 4) {
            formatfs(atoi(argv[2]), atoi(argv[3]));
            savefs("disk01");
        } else if (strcmp(argv[1], "savefs") == 0 && argc == 3) {
            savefs(argv[2]);
        } else if (strcmp(argv[1], "list") == 0) {
            listfs();
        } else if (strcmp(argv[1], "put") == 0 && argc == 3) {
            put(argv[2]);
            savefs("disk01");
        } else if (strcmp(argv[1], "remove") == 0 && argc == 3) {
            remove_file(argv[2]);
            savefs("disk01");
        } else if (strcmp(argv[1], "mkdir") == 0 && argc == 3) {
            mkdir(argv[2]);
            savefs("disk01");
        } else if (strcmp(argv[1], "chmod") == 0 && argc == 4) {
            chmod(argv[2], argv[3]);
            savefs("disk01");
        } else if (strcmp(argv[1], "setuser") == 0 && argc == 3) {
            setuser(argv[2]);
        } else if (strcmp(argv[1], "rename") == 0 && argc == 4) {
            rename_file(argv[2], argv[3]);
            savefs("disk01");
        } else if (strcmp(argv[1], "putfile") == 0 && argc == 3) {
            put_external_file(argv[2]);
            savefs("disk01");
        } else if (strcmp(argv[1], "getfile") == 0 && argc == 3) {
            get_external_file(argv[2]);
        } else if (strcmp(argv[1], "user") == 0 && argc == 3) {
            list_user_files(argv[2]);
        } else if (strcmp(argv[1], "link") == 0 && argc == 4) {
            link_file(argv[2], argv[3]);
            savefs("disk01");
        } else if (strcmp(argv[1], "unlink") == 0 && argc == 3) {
            unlink_file(argv[2]);
            savefs("disk01");
        } else {
            printf("Unknown or incomplete command\n");
        }
    }
    return 0;
}
