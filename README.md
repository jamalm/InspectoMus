# InspectoMus
Author: Jamal Mahmoud
# Description:
A Daemon that monitors the backup and update of a website using 
auditing tools to watch directories and log information. A timer 
is used to begin a backup of the live site to a backup folder and 
then transfer the edits made that day to the live site for the next day.
# Init Phase
1. Create Message Queue, Logger, Timer.
2. Wait for Timer to return true to begin Active Phase.
# Active Phase
1. Lock folders involved in backup/transfer.
2. Wait for response from backup and begin transfer of tagged files.
3. Wait for response from transfer process and begin dormant phase.
4. Log all changes and responses from processes
# Dormant Phase
1. Watch Directories using Auditd and log any changes committed.
2. Wait for Executive order or Timer to begin Active Phase.

# Side Notes:
1. An Admin script is added to start, stop and also to begin a backup at any time for the daemon at the request of root user.

For those that don't know; 
  Daemon: Runs around in the shadows doing devilish deeds
  
