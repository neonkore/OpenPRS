;;;---------------------------------------------------------------------------
;;; To use, put something like the following in your emacs initializations:
;;; --------------------
;;; -- OPRS-Mode
;; (let* ((oprs-home (getenv "OPRS_HOME"))
;;        (oprs-el   (concat oprs-home "/contrib/emacs/oprs-mode")))
;;   (autoload 'prs-opf-mode oprs-el)
;;   (autoload 'prs-inc-mode oprs-el)
;;   (push '("\\.opf$" . prs-opf-mode) auto-mode-alist)
;;   (push '("\\.sym$" . prs-opf-mode) auto-mode-alist)
;;   (push '("\\.inc$" . prs-inc-mode) auto-mode-alist)
;; )
;;;---------------------------------------------------------------------------


;;; -------------------------------------------------------------------------
;;; Code:

(load-library "cl-compat")			;It looks like setf is defined here.

;; NOTE: same as SVN/CVS revision:
(defconst oprs-mode-version "$Revision$"
  "oprs-mode version number.")

;; -------------------------------------------------------------------------
;; Configuration variables --- later might even use customize for these
;; For now, these values can be modified by the user in a .emacs file
;;;---------------------------------------------------------------------------

(defvar prs-agent-name nil
  "The name of the current agent.  Is used in a bunch of places to 
establish connections.")
(defvar prs-opf-mode-hooks nil)
(defvar prs-inc-mode-hooks nil)
(defvar inferior-prs-mode-hooks nil)

;; That should be about it for most users...
;; unless you wanna hack elisp, the rest of this is probably uninteresting


;; -------------------------------------------------------------------------
;; We are built upon some other stuff
(require 'font-lock)
(require 'lisp-mode)
(require 'comint)

;; help determine what emacs we have here...
(defconst oprs-xemacsp (string-match "XEmacs" (emacs-version))
  "Non-nil if we are running in the XEmacs environment.")

;; -------------------------------------------------------------------------
;; mode font faces/definitions

;; make use of font-lock stuff, so say that explicitly
(require 'font-lock)

;; some emacs-en don't define or have regexp-opt available.  
(unless (functionp 'regexp-opt)
  (defmacro regexp-opt (strings)
    "Cheap imitation of `regexp-opt' since it's not availble in this emacs"
    `(mapconcat 'identity ,strings "\\|")))
  

;; -------------------------------------------------------------------------
;; mode font lock specifications/regular-expressions
;;   - for help, look at definition of variable 'font-lock-keywords
;;   - Probably, fontification of command keywords should be in a
;;     different mode than fontification of functions and such.  

(defconst oprs-font-lock-keywords nil
  "Default level highlighting for OPRS mode.")

;;(defconst oprs-font-lock-keywords-1 nil
;;  "Subdued level highlighting for OPRS mode.")
;;
;;(defconst oprs-font-lock-keywords-2 nil
;;  "Medium level highlighting for OPRS mode.")
;;
;;(defconst oprs-font-lock-keywords-3 nil
;;  "Gaudy level highlighting for OPRS mode.")

;; set each of those three variables now..
(let (
      (oprs-keywords-regexp
       (eval-when-compile
         (regexp-opt
          '( ;; Syntactic keywords
	    "if" "then" "else" "elseif" "goto" "while" "do"
	    ))))

      (oprs-commands-regexp
       (eval-when-compile 
         (regexp-opt 
          '( ;; Commands
            "achieve" "action" "all" "applicable" "assert"
            "be"
            "conclude" "condition" "consult" "copyright" "cwp"
            "db" "declare" "delete" "disconnect" "dump" "defop"
            "echo" "empty" "eval" "evaluable" "execute-command"
            "exit" "expression" "extern"
            "fact" "ff" "frame" "function"
            "goal" "graphic"
            "halt" "help"
            "id" "include" "intend" "intention"
            "kernel"
            "list" "load"
            "maintain" "memory" "meta"
            "next"
            "off" "on" "op" "opf" "opfs" "oprs_data_path"
            "parallel" "post" "predicate" "preserve"
            "quit"
            "receive" "relevant" "reload" "require" "reset" "retract" "run"
            "save" "send" "set" "show" "soak" "stat" "status" "step"
            "suc_fail" "symbol"
            "test" "text" "thread" "time_stamping" "trace"
            "undeclare" "unify"
            "variable" "version"
            "wait"
            ))))

      (oprs-evaluable-functions-regexp
       (eval-when-compile
         (regexp-opt
          '( ;; Evaluable Functions
            "integer" "float" "long_long"
            "abs" "mod" "rand" "float-to-int" "int-to-float"
            "make-float-array" "get-float-array"
            "get-float-array-size" "make-int-array" "get-int-array"
            "get-int-array-size" "property-of"
            "fact-invoked-ops-of" "get-the-decision-procedures-of"
            "op-instance-goal" "safety-handlers-of" "preferred-of"
            "goal-statement" "fact-statement" "gstatement-predicat"
            "fstatement-predicat" "gstatement-arg" "fstatement-arg"
            "get-intended-decision-procedures"
            "number-of-intentions" "get-current-intention"
            "get-all-intention" "get-other-intentions"
            "get-sleeping-intentions" "get-root-intentions"
            "find-intention-id" "find-intentions-id"
            "get-intention-priority" "get-intention-time"
            "time" "mtime" "user-clock-tick" "sys-clock-tick"
            "user-sys-clock-tick" "cons" "cons-tail" "first"
            "car" "cdr" "caar" "cadr" "cdar" "cddr"
            "caaar" "cadar" "cdaar" "cddar" "caadr" "caddr" "cdadr"
            "cdddr" "second" "last" "delete-from-list"
            "list-difference" "list-intersection" "list-union"
            "list-difference-order" "length" "select-randonly"
            "reverse" "sort-alpha" "l-list" "gensym" "string-cat"
            "vla" "ff-val" "all" "n-all" "n-all-list" "all-pos"
            "mention" "build-goal" "apply-subst-in-gtexpr"
            "apply-subst-in-goal" "make-and-declare-eval-funct"
            "declare-user-eval-funct"
            ))))

      (oprs-predefined-actions-regexp
       (eval-when-compile
         (regexp-opt
          '( ;; Predefined OPs/actions
            "kernel-name"
            "print" "printf" "print-list" "printf-window" "format" 
            "read-inside"
            "read-inside-id" "read-inside-id-var" "set-float-array"
            "intend-op" "intend-op-with-priority" "intend-op-after"
            "intend-op-with-priority-after"
            "intend-op-before-after" "intend-op-after-before"
            "intend-op-with-priority-after-before"
            "intend-all-ops" "intend-all-ops-as-root"
            "intend-all-ops-after" "intend-all-goals-//"
            "intend-all-goals-//-as-roots"
            "intend-all-goals-//-after-roots" "intend-goal"
            "intend-goal-with-priority" "intend-goal-after-before"
            "intend-goal-with-priority-after-before"
            "tag-current-intention" "kill-other-intentions"
            "kill-intentions" "kill-intention" "asleep-intentions"
            "asleep-intention" "wake-up-intention"
            "asleep-intentions-cond" "set-intention-priority"
            "apply-sort-predicate-to-all" "sort-intention-priority"
            "sort-intention-time" "sort-intention-priority-time"
            "sort-intention-none" 
            "send-message"
            "broadcast-message" "multicast-message" "send-string"
            "execute-command" "start-critical-section"
            "end-critical-section" "fail" "succeed" "test-and-set"
            "make_and_declare_action" "make_and_declare_action"
          ))))

      (oprs-metalevel-regexp
       (eval-when-compile
         (regexp-opt
          '( ;; Metalevel reasoning
            "soak" "applicable-ops-fact" "applicable-ops-goal"
            "fact-invoked-ops" "goal-invoked-ops"
            ))))

      (oprs-kernel-user-functions-regexp
       (eval-when-compile
         (regexp-opt
          '( ;; Kernel user functions
            "start_kernel_user_hook" "end_kernel_user_hook"
            "report_fatal_external_error"
            "report_recoverable_external_error"
            "my_intention_list_scheduler" "my_intention_list_sort"
            ))))

      (oprs-kernel-variables-regexp
       (eval-when-compile
         (regexp-opt
          '( ;; Kernel variables functions
            "intention_scheduler" "intention_par_scheduler"
            ))))
      )

  ;; really simple fontification (strings and comments come for "free")
  (setq oprs-font-lock-keywords
    (list
     ;; Keywords:
     (cons (concat "\\<\\(" oprs-keywords-regexp "\\)\\>")
           'font-lock-keyword-face)
     ;; Commands:
     (cons (concat "\\<\\(" oprs-commands-regexp "\\)\\>")
           'font-lock-keyword-face)
     ;; Evaluable functions:
     (cons (concat "\\<\\(" oprs-evaluable-functions-regexp "\\)\\>")
           'font-lock-function-name-face)
     ;; Predefined Actions:
     (cons (concat "\\<\\(" oprs-predefined-actions-regexp "\\)\\>")
           'font-lock-function-name-face)
     ;; Metalevel reasoning
     (cons (concat "\\<\\(" oprs-metalevel-regexp "\\)\\>")
           'font-lock-constant-face)
     ;; Kernel user functions
     (cons (concat "\\<\\(" oprs-kernel-user-functions-regexp "\\)\\>")
           'font-lock-function-name-face)
     ;; Kernel variables
     (cons (concat "\\<\\(" oprs-kernel-variables-regexp "\\)\\>")
           'font-lock-variable-name-face)

     ;; Types:
     ;; (cons (concat "\\<\\(" oprs-types "\\)\\>")
     ;;       'font-lock-type-face)

     ;; Special constants:
     ;; '("\\<_\\(np\\|pid\\|last\\)\\>" . font-lock-reference-face)

     ;; Symbols/shorthand
     ;; should probably ensure that these only are highlighted
     ;; if immediately following an open-paren
     (cons (regexp-opt '("!" "?" "~" "=" "+" "-" ">" "<"
                         "=>" "~>" "&" "||" "^" "//" "#" "%"))
           'font-lock-constant-face)

     ;; User variables, prefixed by $ or @
     '("[$@][-_A-Za-z0-9]+" . font-lock-variable-name-face)

     ;; Tokens. 
     ;; I think the grammar of these things is a bit richer than this,
     ;; but this will do for now.
     '("|[^|]+|" . font-lock-constant-face)

     ;; Comments
     '(";.+$" . font-lock-comment-face)
   )))


;; -------------------------------------------------------------------------
;; "install" oprs-mode font lock specifications

;; FMI: look up 'font-lock-defaults
(defconst oprs-font-lock-defaults
  '(
    (oprs-font-lock-keywords 
;     oprs-font-lock-keywords-1
;     oprs-font-lock-keywords-2
;     oprs-font-lock-keywords-3
     )                                    ;; font-lock stuff (keywords)
    nil                                   ;; keywords-only flag
    t                                     ;; case-fold keyword searching
    ;;((?_ . "w") (?$ . "."))             ;; mods to syntax table
    nil                                   ;; mods to syntax table (see below)
    nil                                   ;; syntax-begin
;   (font-lock-mark-block-function . mark-defun)
    )
)

;; "install" the font-lock-defaults based upon version of emacs we have
(cond (oprs-xemacsp
       (put 'prs-opf-mode 'font-lock-defaults oprs-font-lock-defaults)
       (put 'prs-inc-mode 'font-lock-defaults oprs-font-lock-defaults))
      ;; else some form of fsf-emacs:
;; this form is obsolete - just set buffer-local-variable in mode
;;      ((not (assq 'prs-opf-mode font-lock-defaults-alist))
;;       (setq font-lock-defaults-alist
;;             (cons
;;              (cons 'prs-opf-mode oprs-font-lock-defaults)
;;              font-lock-defaults-alist)))
      )


;; -------------------------------------------------------------------------
;; prs-opf-mode vars

(defvar prs-common-syntax-table
;;  "Syntax table in use in OPRS-mode buffers."
    (let ((result (make-syntax-table)))
      (modify-syntax-entry ?- "w" result)
      (modify-syntax-entry ?_ "w" result)
      (modify-syntax-entry ?\; "<" result)
      (modify-syntax-entry ?\n ">" result)
      result))

(defvar prs-opf-mode-syntax-table
;;  "Syntax table in use in OPRS-mode buffers."
    (let ((result (copy-syntax-table prs-common-syntax-table)))
      result))

(defvar prs-opf-mode-map
;;  "Keymap for OPRS-mode."
    (let ((map (make-sparse-keymap)))
      (define-key map "\t" 		'lisp-indent-line)
      (define-key map (kbd "C-j") 	'newline-and-indent)
      (define-key map (kbd "<return>") 	'newline-and-indent)
      (define-key map (kbd "<newline>") 'newline-and-indent)
      (define-key map (kbd "C-c C-q") 	'indent-sexp)
      (define-key map (kbd "C-c q") 	'indent-sexp)
      map))

(defvar prs-opf-abbrev-table nil
  "*Abbrev table in use in OPRS-mode buffers.")
(if prs-opf-abbrev-table
    nil
  (define-abbrev-table 'prs-opf-abbrev-table
    '(
;; Commented out for now - need to think about what abbrevs make sense
;;     ("assert" 	"ASSERT" 	oprs-check-expansion 0)
      )))


;; -----------------------------------------------------------------
;; PRS-OPF-Mode, finally

(defun prs-op-mode ()
  "Wrapper around prs-opf-mode for backward compatibility"
  (interactive)
  (prs-opf-mode))

(defun prs-opf-mode ()
  "Emacs mode for PRS OPF files."
  (interactive)
  (kill-all-local-variables)
  (setf mode-name               "PRS/opf"
        major-mode              'prs-opf-mode
        local-abbrev-table      prs-opf-abbrev-table)
  (use-local-map                prs-opf-mode-map)
  (set-syntax-table             prs-opf-mode-syntax-table)

  ;; Make local variables
  (make-local-variable 'case-fold-search)
  (make-local-variable 'parse-sexp-ignore-comments)
  (make-local-variable 'font-lock-defaults)
  ;; Now set their values
  (setq case-fold-search                t
        parse-sexp-ignore-comments 	t
	font-lock-defaults		oprs-font-lock-defaults
  )

  ;; Turn on font-lock mode
  (font-lock-mode t)

  ;; Finally, run the hooks and be done.
  (run-hooks 'prs-opf-mode-hooks))


;; -------------------------------------------------------------------------
;; prs-inc-mode vars

(defvar prs-inc-mode-map
    (let ((result (make-sparse-keymap)))
      result))

(defvar prs-inc-mode-syntax-table
    (let ((result (copy-syntax-table prs-common-syntax-table)))
      result))

(defvar prs-inc-abbrev-table nil)
(define-abbrev-table 'prs-inc-abbrev-table ())

;;; -----------------------------------------------------------------
;; PRS-INC-Mode

(defun prs-inc-mode ()
  "Emacs mode for PRS INC files.
Set a file variable of PRS-AGENT-NAME to define a name for the
agent into which this file will be loaded."
  (interactive)
  (kill-all-local-variables)
  (setf mode-name               "PRS/inc"
        major-mode              'prs-inc-mode
        local-abbrev-table      prs-inc-abbrev-table)
  (use-local-map                prs-inc-mode-map)
  (set-syntax-table             prs-inc-mode-syntax-table)

  ;; Make local variables
  (make-local-variable 'prs-agent-name)
  (make-local-variable 'case-fold-search)
  (make-local-variable 'parse-sexp-ignore-comments)
  (make-local-variable 'font-lock-defaults)
  ;; Now set their values
  (setq case-fold-search                t
        parse-sexp-ignore-comments     t
       font-lock-defaults              oprs-font-lock-defaults
  )

  ;; Turn on font-lock mode
  (font-lock-mode t)

  ;; Finally, run the hooks and be done.
  (run-hooks 'prs-inc-mode-hooks))


;; -------------------------------------------------------------------------
;; Inferior PRS mode
;; This stuff is all wrong --- needs to use comint mode stuff.  FIXME
(define-derived-mode inferior-prs-mode comint-mode "Inferior PRS"
 "Major mode for interacting with an inferior open PRS server.

   Entry into the mode runs inferior-prs-mode-hooks.
   \\{inferior-prs-mode-map}"
  ;; don't need to explicitly define abbrev table or inferior-prs-mode-map
  (run-hooks inferior-prs-mode-hooks))


;; -------------------------------------------------------------------------
;; PRS process code

(defconst +prs-buffer-name+ "*oprs*")
(defvar prs-prs-server-name "oprs-server")
(defvar prs-prs-server-program-switches '("-l upper"))
(defvar prs-prs-message-passer "mp-oprs")
(defvar prs-prs-message-passer-killer "kill-mp")
(defvar prs-prs-kernel-program-name "oprs")
(defvar prs-prs-kernel-program-switches '("-l upper"))
(defvar *prs-mp-port-number* 3300)
(defvar *prs-mp-host* nil
  "What host should open-prs look for its message-passer
on.  Will look on localhost if NIL.")
(defvar prs-server-prompt-regexp "oprs-server>")

(defun run-prs (arg)
  "Run an inferior Open PRS server process, input and output via buffer 
*oprs*.  With prefix argument ARG, restart the prs server process if running 
before."
  (interactive "P")
  (if (and arg (get-process "oprs"))
      (progn
        (process-send-string "oprs" "quit")
        (while (get-process "oprs") (sit-for 0.1))))
  (let ((buff (buffer-name)))
    (if (not (string= buff +prs-buffer-name+))
        (prs-goto-prs-process-buffer))
    ;; not obvious we need extensive variables here...
    ;; (prolog-mode-variables)
    (prs-ensure-process)
    ))

(defun prs-goto-prs-process-buffer ()
  "Switch to the prolog process buffer and go to its end."
  (switch-to-buffer-other-window +prs-buffer-name+)
  (goto-char (point-max))
)

(defun prs-start-processes ()
  ;; check for or make a message-passer
  (prs-start-mp)
  ;; start the process...
  (let ((switches prs-prs-server-program-switches))
    (setq switches
          (append switches `("-j" ,(number-to-string *prs-mp-port-number*))))
    (if *prs-mp-host*
        (setq switches (append switches
                               `("-m" ,*prs-mp-host*))))
    (apply 'make-comint "prs" prs-prs-server-name nil
           switches)))

;;; for now there's no good way to do this, since the open-prs
;;; codebase does not support a "ping" to an MP.  Lose lose.
;;; So we just hope it's there for us. [2010/02/04:rpg]
(defun prs-mp-unfound ()
  "Returns T if it is not possible to find a 
message passer running using *prs-mp-port-number*
on *prs-mp-host*"
  nil
  )

(defun prs-start-mp (&optional always)
  "Check for the presence of an open-prs message passer on
*prs-mp-port-number* on *prs-mp-host* and start one if it's
not there.  If the optional argument ALWAYS is non-NIL,
kill any existing MP and start a new one."
  (let ((process-killed nil))
    (if always
        (call-process prs-prs-message-passer-killer)
        (setq process-killed t))
    (if (or process-killed
            (prs-mp-unfound))
        (progn
          (if *prs-mp-host*
              (error "Can't start an open-prs message passer remotely on host %s" *prs-mp-host*))
          (start-process "mp" "*oprs-mp*" prs-prs-message-passer
                         "-j" (number-to-string *prs-mp-port-number*))))))

(defun prs-ensure-process (&optional wait)
  "If open-prs process is not running, run it.
If the optional argument WAIT is non-nil, wait for prs prompt specified by
the variable `prolog-prompt-regexp'."
  (if (comint-check-proc +prs-buffer-name+)
      ()
    (prs-start-processes)
    (save-excursion
      (set-buffer +prs-buffer-name+)
      (inferior-prs-mode)
      (if wait
          (progn
            (goto-char (point-max))
            (while
                (save-excursion
                  (not
                   (re-search-backward
                    (concat "\\(" prs-server-prompt-regexp "\\)" "\\=")
                    nil t)))
              (sit-for 0.1)))))))

;;; FIXME: should be fixed to interactively take a filename (of an inc file) and
;;; an agent name.  Currently it just tries to get the agent name
;;; from a file variable.
(defun prs-start-agent (filename)
  "Start an agent running, initialized with the
contents of the current inc file.
   For use in prs-inc-mode.
   Should be fixed to interactively take an
agent name."
  (let* ((agent-name
          (if (boundp 'prs-agent-name) prs-agent-name
              (symbol-name (gensym "prs-agent-"))))
         (buf-name (concat "*" agent-name "*"))
         (switches prs-prs-kernel-program-switches))
    (if (or (not (boundp 'prs-agent-name))
            (null prs-agent-name))
      (setq prs-agent-name agent-name))
    (setq switches
          (append switches `("-j" ,(number-to-string *prs-mp-port-number*))))
    (if *prs-mp-host*
        (setq switches (append switches
                               `("-m" ,*prs-mp-host*))))
    (setq switches (append switches `("-x" ,filename)))
    (setq switches (append switches (list agent-name)))
    
    (apply 'start-process agent-name buf-name prs-prs-kernel-program-name
           switches)))

;; -------------------------------------------------------------------------
;;###autoload

(provide 'oprs-mode)

;; EOF oprs-mode.el

