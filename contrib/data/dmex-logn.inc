;;;;;;;;;;;;;;;;;;;;;;;;;;; -*- Mode: Emacs-Lisp -*- ;;;;;;;;;;;;;;;;;;;;;;;;;;
;; dmex-ln.inc -- Distributed Mutual Exclusion library.
;; 
;; Copyright (C) 1996 Francois Felix Ingrand
;; 
;; Author          : Francois Felix Ingrand
;; Created On      : Mon Oct 28 14:48:59 1996
;; Last Modified By: Francois Felix Ingrand
;; Last Modified On: Thu Mar 20 16:38:50 1997
;; Update Count    : 7
;; Status          : OK
;;
;; $Id$
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; This library implement Trehel and Naimi 
;; "A Log(n) Distributed Mutual Exclusion Algorithm Based on Path Reversal"
;; 
;; This version is a *and* allocation version (we totaly sort resources before
;; requesting them).
;;
;; Usage:
;;  each agent must have a name
;;  add (my-name r1)
;;  At initialisation, each resource is given to a specific agent (the other
;;  one memorize the name of this "father")
;;  to initialize the "resource set" to agent r1
;;  add (! (dmex-ln-init r1  (. foo1 foo2 foo3  bar1 bar2 bar3 bar4 boo1 boo2 boo3 boo4 boo5  .)))
;; then
;; (! (dmex-ln-request-rsc (. foo1 bar2 boo3 .)))
;; and
;; (! (dmex-ln-release-rsc (. foo1 bar2 boo3 .)))
;; will release them.
;;
;; Note:
;; - the test subdirectory contains a test example with 5 "robots".
;; - a new agent can join without any problem...


require "semaphore.inc"

include "dmex-logn.sym"
load opf "dmex-logn.opf"
