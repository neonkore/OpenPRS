declare ff POSITION 1
load opf "mini-default.opf"
set meta off
;;; trace all off
load opf "ff-val.opf"
trace graphic op TEST on
conclude (POSITION A 123123.123123)
