;;;;;;;;;;;;;;;;;;;;;;;;;;; -*- Mode: Emacs-Lisp -*- ;;;;;;;;;;;;;;;;;;;;;;;;;;
;; dmex.inc -- Distributed Mutual Exclusion library.
;; Programmed and contributed by Samer Qutub (LAAS/CNRS)
;; Modified by felix...
;; 
;; Copyright (C) 1996 Samer Qutub, LAAS/CNRS.
;; 
;; Author          : Samer Qutub (sam@laas.fr)
;; Created On      : Mon Sep 30 15:31:45 1996
;; Last Modified By: Francois Felix Ingrand
;; Last Modified On: Thu Mar 20 16:38:49 1997
;; Update Count    : 14
;; Status          : OK
;; 
;; $Id$
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; This library implement Michel Raynal's Distributed k-out of-M resources
;; allocation problem, described in the paper:
;; "A Distributed solution to the k-out of-M resources allocation problem."
;; 
;; This version is the *and* allocation (no message reduction in this version).
;;
;; Usage:
;;  each agent must have a name and a priority (they MUST be different...)
;;  add (my-name r1)
;;  add (my-dmex-prio 1)
;;  a list of all the agents in the protocol
;;  add (connected-agents (. r1 r2 r3 r4 r5 .))
;;  to initialize the "resource set" (. resource-name size .)
;;  add (! (dmex-create (. (. foo 10 .) (. bar 20 .) (. boo 30 .) .)))
;; then
;; (! (dmex-request-rsc (. foo (. bar 10 .) (. boo 25 .) .)))
;; will return when 1 foo, 10 bar(s) and 25 boo(s) are available
;; (! (dmex-release-rsc (. foo (. bar 10 .) (. boo 25 .) .)))
;; will release them.
;;
;; Note:
;; - one can only release when the resource has been allocated.
;; - one can only release the exact same value then requested per
;;   resource (if asked 15 foo, you must release 15, not 10).
;; - one can release resources independantly. (if you have asked
;;   simultaneously some foo *and* bar *and* boo, you can release the 
;;   boo then the bar then the foo.
;; - the test subdirectory contains a test example with 5 "robots".
;;

require "semaphore.inc"
require "bk-point.inc"

include "dmex.sym"
load opf "dmex.opf"
