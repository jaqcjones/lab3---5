/*
 * Mutex assembly code
 */

	.syntax unified
	.thumb

	.global lock_mutex
	.global unlock_mutex

	.text


/*
 * Lock mutex variable
 * Return in r2 success (0) / failure (1)
 */
// .lcomm mutex_var,4
 .equ locked,	1
 .equ unlocked,	0
lock_mutex:
	push {r1,r3,lr}
	ldr r1, =locked
	ldr r3, [r0]		/* Mutex variable passed into r0 */
 check_lock:
	ldrex r2,[r3]
	cmp r2, r1					/* Check if mutex locked */
	beq wait_for_update			/* Abort if locked */
	strex r2,r1,[r3]		/* Attempt to lock if unlocked */
	cmp r2, #1					/* Check if attempt failed */
	beq check_lock
	/* Lock acquired */
	dmb
 wait_for_update:
 	str r0, [r2]			/* Return 0/1 for success/failure */
	pop {r1,r3,pc}

unlock_mutex:
	push {r1,r2,lr}
	ldr r1, =unlocked
	ldr r2, [r0]
	dmb
	str r1,[r2]
	pop {r1,r2,pc}

	.end
