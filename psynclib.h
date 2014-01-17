/* Copyright (c) 2013 Anton Titov.
 * Copyright (c) 2013 pCloud Ltd.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of pCloud Ltd nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL pCloud Ltd BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PSYNC_LIB_H
#define _PSYNC_LIB_H

/* All paths are in UTF-8 regardless of the OS. */

#include <stdint.h>
#include <stdlib.h>

typedef uint64_t psync_folderid_t;
typedef uint64_t psync_fileid_t;
typedef uint32_t psync_syncid_t;
typedef uint32_t psync_eventtype_t;
typedef uint32_t psync_synctype_t;
typedef uint32_t psync_listtype_t;

typedef struct {
  psync_fileid_t fileid;
  uint64_t size;
} pfile_t;

typedef struct {
  psync_folderid_t folderid;
  uint8_t cansyncup;
  uint8_t cansyncdown;
} pfolder_t;

typedef struct {
  const char *name;
  union {
    pfolder_t folder;
    pfile_t file;
  };
  uint16_t namelen;
  uint8_t isfolder;
} pentry_t;

typedef struct {
  size_t entrycnt;
  pentry_t entries[];
} pfolder_list_t;

#define PSTATUS_READY                   0
#define PSTATUS_DOWNLOADING             1
#define PSTATUS_UPLOADING               2
#define PSTATUS_DOWNLOADINGANDUPLOADING 3
#define PSTATUS_LOGIN_REQUIRED          4
#define PSTATUS_BAD_LOGIN_DATA          5
#define PSTATUS_ACCOUNT_FULL            6
#define PSTATUS_DISK_FULL               7
#define PSTATUS_PAUSED                  8
#define PSTATUS_STOPPED                 9
#define PSTATUS_OFFLINE                10
#define PSTATUS_CONNECTING             11
#define PSTATUS_SCANNING               12
#define PSTATUS_USER_MISMATCH          13

typedef struct {
  uint64_t bytestoupload; /* sum of the sizes of files that need to be uploaded to sync state */
  uint64_t bytestouploadcurrent; /* sum of the sizes of files in filesuploading */
  uint64_t bytesuploaded; /* bytes uploaded in files accounted in filesuploading */
  uint64_t bytestodownload; /* sum of the sizes of files that need to be downloaded to sync state */
  uint64_t bytestodownloadcurrent; /* sum of the sizes of files in filesdownloading */
  uint64_t bytesdownloaded; /* bytes downloaded in files accounted in filesdownloading */
  uint32_t status; /* current status, one of PSTATUS_ constants */
  uint32_t filestoupload; /* number of files to upload in order to sync state, including filesuploading*/
  uint32_t filesuploading; /* number of files currently uploading */
  uint32_t uploadspeed; /* in bytes/sec */
  uint32_t filestodownload;  /* number of files to download in order to sync state, including filesdownloading*/
  uint32_t filesdownloading; /* number of files currently downloading */
  uint32_t downloadspeed; /* in bytes/sec */
  uint8_t remoteisfull; /* account is full and no files will be synced upwards*/
  uint8_t localisfull; /* (some) local hard drive is full and no files will be synced from the cloud */
} pstatus_t;

/* PEVENT_LOCAL_FOLDER_CREATED means that a folder was created in remotely and this action was replicated
 * locally, not the other way around. Accordingly PEVENT_REMOTE_FOLDER_CREATED is fired when locally created
 * folder is replicated to the server.
 */

#define PEVENT_TYPE_LOCAL            (0<<0)
#define PEVENT_TYPE_REMOTE           (1<<0)
#define PEVENT_TYPE_FILE             (0<<1)
#define PEVENT_TYPE_FOLDER           (1<<1)
#define PEVENT_TYPE_CREATE           (0<<2)
#define PEVENT_TYPE_DELETE           (1<<2)
#define PEVENT_TYPE_RENAME           (2<<2)
#define PEVENT_TYPE_START            (0<<5)
#define PEVENT_TYPE_FINISH           (1<<5)

#define PEVENT_LOCAL_FOLDER_CREATED   (PEVENT_TYPE_LOCAL+PEVENT_TYPE_FOLDER+PEVENT_TYPE_CREATE)
#define PEVENT_REMOTE_FOLDER_CREATED  (PEVENT_TYPE_REMOTE+PEVENT_TYPE_FOLDER+PEVENT_TYPE_CREATE)
#define PEVENT_FILE_DOWNLOAD_STARTED  (PEVENT_TYPE_LOCAL+PEVENT_TYPE_FILE+PEVENT_TYPE_CREATE+PEVENT_TYPE_START)
#define PEVENT_FILE_DOWNLOAD_FINISHED (PEVENT_TYPE_LOCAL+PEVENT_TYPE_FILE+PEVENT_TYPE_CREATE+PEVENT_TYPE_FINISH)
#define PEVENT_FILE_UPLOAD_STARTED    (PEVENT_TYPE_REMOTE+PEVENT_TYPE_FILE+PEVENT_TYPE_CREATE+PEVENT_TYPE_START)
#define PEVENT_FILE_UPLOAD_FINISHED   (PEVENT_TYPE_REMOTE+PEVENT_TYPE_FILE+PEVENT_TYPE_CREATE+PEVENT_TYPE_FINISH)
#define PEVENT_LOCAL_FOLDER_DELETED   (PEVENT_TYPE_LOCAL+PEVENT_TYPE_FOLDER+PEVENT_TYPE_DELETE)
#define PEVENT_REMOTE_FOLDER_DELETED  (PEVENT_TYPE_REMOTE+PEVENT_TYPE_FOLDER+PEVENT_TYPE_DELETE)
#define PEVENT_LOCAL_FILE_DELETED     (PEVENT_TYPE_LOCAL+PEVENT_TYPE_FILE+PEVENT_TYPE_DELETE)
#define PEVENT_REMOTE_FILE_DELETED    (PEVENT_TYPE_REMOTE+PEVENT_TYPE_FILE+PEVENT_TYPE_DELETE)
#define PEVENT_LOCAL_FOLDER_RENAMED   (PEVENT_TYPE_LOCAL+PEVENT_TYPE_FOLDER+PEVENT_TYPE_RENAME)

#define PSYNC_DOWNLOAD_ONLY  1
#define PSYNC_UPLOAD_ONLY    2
#define PSYNC_FULL           3
#define PSYNC_SYNCTYPE_MIN   1
#define PSYNC_SYNCTYPE_MAX   3

#define PERROR_LOCAL_FOLDER_NOT_FOUND   1
#define PERROR_REMOTE_FOLDER_NOT_FOUND  2
#define PERROR_DATABASE_OPEN            3
#define PERROR_NO_HOMEDIR               4
#define PERROR_SSL_INIT_FAILED          5
#define PERROR_DATABASE_ERROR           6
#define PERROR_LOCAL_FOLDER_ACC_DENIED  7
#define PERROR_REMOTE_FOLDER_ACC_DENIED 8
#define PERROR_FOLDER_ALREADY_SYNCING   9
#define PERROR_INVALID_SYNCTYPE        10

#define PLIST_FILES   1
#define PLIST_FOLDERS 2
#define PLIST_ALL     3

typedef struct {
  const char *localname;
  const char *localpath;
  const char *remotename;
  const char *remotepath;
  psync_folderid_t folderid;
  psync_syncid_t syncid;
  psync_synctype_t synctype;
} psync_folder_t;

typedef struct {
  size_t foldercnt;
  psync_folder_t folders[];
} psync_folder_list_t;

#define PSYNC_INVALID_SYNCID (psync_syncid_t)-1

#ifdef __cplusplus
extern "C" {
#endif
  
typedef void *(*psync_malloc_t)(size_t);
typedef void *(*psync_realloc_t)(void *, size_t);
typedef void (*psync_free_t)(void *);

extern psync_malloc_t psync_malloc;
extern psync_realloc_t psync_realloc;
extern psync_free_t psync_free;
  
/* Status change callback is called every time value is changed. It may be called quite often
 * when there are active uploads/downloads. Callbacks are issued from a special callback thread
 * (e.g. the same thread all the time) and are guaranteed not to overlap.
 */
  
typedef void (*pstatus_change_callback_t)(pstatus_t *status);


/* Event callback is called every time a download/upload is started/finished.
 * It is unsafe to use pointers to strings that are passed as parameters after
 * the callback return, if you need to use them this way, strdup() will do the
 * job. Event callbacks will not overlap.
 */

typedef void (*pevent_callback_t)(psync_eventtype_t event, psync_syncid_t syncid, const char *name, const char *localpath, const char *remotepath);

/* psync_init inits the sync library. No network or local scan operations are initiated
 * by this call, call psync_start_sync to start those. However listing remote folders,
 * listing and editing syncs is supported.
 * 
 * Returns 0 on success and -1 otherwise.
 * 
 * psync_start_sync starts remote sync, both callbacks can be NULL, but most of the time setting
 * at least status_callback will make sense. Applications should expect immediate
 * status_callback with status of PSTATUS_LOGIN_REQUIRED after first run of psync_start_sync().
 * 
 * psync_download_state is to be called after psync_init but before/instead of psync_start_sync.
 * This function downloads the directory structure into the local state in foreground (e.g. it can
 * take time to complete). It returns one of PSTATUS_-es, specifically PSTATUS_READY, PSTATUS_OFFLINE
 * or one of login-related statuses. After a successful call to this function remote folder listing can
 * be preformed.
 * 
 * psync_destroy is to be called before application exit. This is not neccessary.
 * In any case psync_destroy will return relatively fast, regardless of blocked
 * network calls and other potentially slow to finish tasks.
 * 
 * psync_set_alloc can set the allocator to be used by the library. To be called
 * BEFORE psync_init if ever. If allocator is provided, its free() function is to
 * be used to free any memory that is said to be freed when returned by the library.
 * 
 * psync_set_database_path can set a full path to database file. If it does not exists
 * it will be created. The function should be only called before psync_init. If
 * database path is not set, appropriate location for the given OS will be chosen.
 * The library will make it's own copy of the path, so the memory can be free()d/reused
 * after the function returns. The path is not checked by the function itself, if it is
 * invalid (directory does not exist, it is not writable, etc) psync_init will return
 * -1 and the error code will be PERROR_DATABASE_OPEN. In this condition it is safe to
 * call psync_set_database_path and psync_init again. A special value of ":memory:" for 
 * databasepath will create in-memory database that will not be preserved between runs.
 * An empty string will create the database in a temporary file, the net effect being
 * similar to the ":memory:" option with less pressure on the required memory. The
 * underlying database is in fact SQLite, so any other options that work for SQLite will
 * work here.
 * 
 */

void psync_set_database_path(const char *databasepath);
void psync_set_alloc(psync_malloc_t malloc_call, psync_realloc_t realloc_call, psync_free_t free_call);

int psync_init();
void psync_start_sync(pstatus_change_callback_t status_callback, pevent_callback_t event_callback);
uint32_t psync_download_state();
void psync_destroy();

/* returns current status.
 */

void psync_get_status(pstatus_t *status);

/* psync_set_user_pass and psync_set_auth functions can be used for initial login 
 * (PSTATUS_LOGIN_REQUIRED) and when PSTATUS_BAD_LOGIN_DATA error is returned, however
 * if the username do not match previously logged in user, PSTATUS_USER_MISMATCH event
 * will be generated. Preferably on PSTATUS_BAD_LOGIN_DATA the user should be only prompted
 * for new password and psync_set_pass should be called. To change the current user, psync_unlink
 * is to be called first and then the new user may log in.
 * 
 * The pointer returned by psync_get_username() is to be free()d.
 */

char *psync_get_username();
void psync_set_user_pass(const char *username, const char *password, int save);
void psync_set_pass(const char *password, int save);
void psync_set_auth(const char *auth, int save);
void psync_unlink();

/* psync_add_sync_by_path and psync_add_sync_by_folderid are to be used to add a folder to be synced,
 * on success syncid is returned, on error -1. The value of synctype should always be one of PSYNC_DOWNLOAD_ONLY,
 * PSYNC_UPLOAD_ONLY or PSYNC_FULL.
 * 
 * psync_change_synctype changes the sync type, on success returns 0 and -1 on error.
 * 
 * psync_delete_sync deletes the sync relationship between folders, on success returns 0
 * and -1 on error (it is only likely to fail if syncid is invalid). No files or folders
 * are deleted either in the cloud or locally.
 * 
 * psync_get_sync_list returns all folders that are set for sync. On error returns NULL.
 * On success the returned pointer is to be free()d.
 * 
 */

psync_syncid_t psync_add_sync_by_path(const char *localpath, const char *remotepath, psync_synctype_t synctype);
psync_syncid_t psync_add_sync_by_folderid(const char *localpath, psync_folderid_t folderid, psync_synctype_t synctype);
int psync_change_synctype(psync_syncid_t syncid, psync_synctype_t synctype);
int psync_delete_sync(psync_syncid_t syncid);
psync_folder_list_t *psync_get_sync_list();

/* Use the following functions to list local or remote folders.
 * For local folders fileid and folderid will have undefined values.
 * Remote paths use slashes (/) and start with one.
 * In case of success the returned folder list is to be freed with a
 * single call to free(). In case of error NULL is returned. Parameter
 * listtype should be one of PLIST_FILES, PLIST_FOLDERS or PLIST_ALL.
 * 
 * Folders do not contain "." or ".." entries.
 * 
 * All files/folders are listed regardless if they are to be ignored 
 * based on 'ignorepatterns' setting. If needed, pass the names to
 * psync_is_name_to_ignore that returns 1 for files tha are to be
 * ignored and 0 for others.
 * 
 * Remote root folder has 0 folderid.
 */

pfolder_list_t *psync_list_local_folder_by_path(const char *localpath, psync_listtype_t listtype);
pfolder_list_t *psync_list_remote_folder_by_path(const char *remotepath, psync_listtype_t listtype);
pfolder_list_t *psync_list_remote_folder_by_folderid(psync_folderid_t folderid, psync_listtype_t listtype);
int psync_is_name_to_ignore(const char *name);

/* Returns the code of the last error that occured when calling psync_* functions
 * in the given thread. The error is one of PERROR_* constants.
 */

uint32_t psync_get_last_error();

/* Pause stops the sync, but both local and remote directories are still
 * monitored for updates and status updates are still received with updated
 * filestoupload/filestodownload and others.
 * 
 * Stop stops all the actions of the library. No network traffic and no local
 * scans are to be expected after this call. No status updates callback except
 * the one setting PSTATUS_STOPPED status.
 * 
 * Resume will restart all operations in both paused and stopped state.
 */

int psync_pause();
int psync_stop();
int psync_resume();

/* Regsiters a new user account. email is user e-mail address which will also be
 * the username after successful registration. Password is user's chosen password
 * implementations are advised to have the user verify the password by typing it
 * twice. The termsaccepted field should only be set to true if the user actually
 * indicated acceptance of pCloud terms and conditions.
 *
 * Returns zero on success, -1 if network error occurs or a positive error code from
 * this list:
 * https://docs.pcloud.com/methods/auth/register.html
 * In case of error. 
 * 
 * If err is not NULL in all cases of non-zero return it will be set to point to a
 * psync_malloc-allocated buffer with English language error text, suitable to display
 * to the user. This buffer must be freed by the application.
 * 
 */

int psync_register(const char *email, const char *password, int termsaccepted, char **err);

/* 
 * 
 * List of settings: 
 * usessl (bool) - use SSL connections to remote servers 
 * maxdownloadspeed (int) - maximum download speed in bytes per second, 0 for auto-shaper, -1 for no limit
 * maxuploadspeed (int) - maximum upload speed in bytes per second, 0 for auto-shaper, -1 for no limit
 * ignorepatterns (string) - patterns of files and folders to be ignored when syncing, separated by ";" supported widcards are
 *                          * - matches any number of characters (even zero)
 *                          ? - matches exactly one character
 * 
 */


  
#ifdef __cplusplus
}
#endif

#endif