#
# tcl client to message passer
#
# Copyright (c) 2005 Gautier hattenberger
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    - Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    - Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#


namespace eval mp {    

    variable REGISTER_OK 0
    variable REGISTER_NAME_CONFLICT 1
    variable REGISTER_DENIED 2
    variable MESSAGE_PROTOCOL 0
    variable MESSAGE_MT 1
    variable BROADCAST_MT 2
    variable MULTICAST_MT 3
    variable DISCONNECT_MT 4

    variable mpSocket

    proc connect {clientname host port} {
        variable mpSocket 
        variable MESSAGE_PROTOCOL 
	variable REGISTER_OK
	variable REGISTER_NAME_CONFLICT
	variable REGISTER_DENIED

        puts "connection to $host:$port with name $clientname ... "
        
        # open socket
        set mpSocket [socket $host $port]

        fconfigure $mpSocket -translation binary

        puts -nonewline $mpSocket [binary format I1I1a* \
				       $MESSAGE_PROTOCOL \
                                       [string length $clientname] \
				       $clientname]
        flush $mpSocket

        puts "waiting for register..."
        
        binary scan [read $mpSocket 4] I1 register

        puts "connection register: $register"

        if "$register != $REGISTER_OK" {
            close $mpSocket
            switch $register {
                $REGISTER_NAME_CONFLICT {
                    puts "name \"$clientname\" is already used for another client."
                }
                $REGISTER_DENIED {
                    puts "Permission denied."
                }
                default {
                    puts "Unknown error (\"$register\")."
                }
            }
	    return
        }
#         fconfigure $mpSocket -translation "auto auto"
#         fconfigure $mpSocket -blocking 0
    }

    proc disconnect {} {
        variable mpSocket 
        variable DISCONNECT_MT
        puts $mpSocket $DISCONNECT_MT
        close $mpSocket
        puts "disconnected"
    }

    proc send {recipient message} {
	variable mpSocket
	variable MESSAGE_MT
	
	puts -nonewline $mpSocket [binary format I1I1a* \
				       $MESSAGE_MT \
				       [string length $recipient] \
				       $recipient]
	puts -nonewline $mpSocket [binary format I1a* \
				       [string length $message] \
				       $message]
 	flush $mpSocket
    }

    proc broadcast {message} {
	variable mpSocket
	variable BROADCAST_MT

	puts -nonewline $mpSocket [binary format I1 \
				       $BROADCAST_MT]
	puts -nonewline $mpSocket [binary format I1a* \
				       [string length $message] \
				       $message]
 	flush $mpSocket
    }

    proc receive {} {
	variable mpSocket

	gets $mpSocket message
	return $message
    }

}
