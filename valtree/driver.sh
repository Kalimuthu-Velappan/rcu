#!/bin/sh

# Driver script for RCU testing under Nidhugg.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, you can access it online at
# http://www.gnu.org/licenses/gpl-2.0.html.
#
# Author: Michalis Kokologiannakis <mixaskok@gmail.com>

unroll=5

# runfailure <kernel_version> --<memory_model> <source_file> CFLAGS
#
# Run Nidhugg on the specified <source_file>, under the memory model
# specified by <memody_model>, on Linux kernel version <kernel_version>,
# with the expectation that verification will fail.
# Additional arguments will be passed to the clang compiler (see test
# files for more information).
runfailure() {
    kernel_version=$1
    mem_model=$2
    test_file=$3
    shift 3
    
    echo '--------------------------------------------------------------------'
    echo '--- Preparing to run tests on kernel' ${kernel_version}
    echo '--- Expecting verification failure'
    echo '--------------------------------------------------------------------'
    if nidhuggc -I ${kernel_version} -std=gnu99 $* -- ${mem_model} \
	     --extfun-no-race=fprintf --print-progress-estimate \
	     --disable-mutex-init-requirement --unroll=${unroll} ${test_file}
    then
	echo '^^^ Unexpected verification success'
	failure=1
    fi
}

# runsuccess <kernel_version> --<memory_model> <source_file> CFLAGS
#
# Run Nidhugg on the specified <source_file>, under the memory model
# specified by <memody_model>, on Linux kernel version <kernel_version>,
# with the expectation that verification will succeed.
# Additional arguments will be passed to the clang compiler (see test
# files for more information).
runsuccess() {
    kernel_version=$1
    mem_model=$2
    test_file=$3
    shift 3
    
    echo '--------------------------------------------------------------------'
    echo '--- Preparing to run tests on kernel' ${kernel_version}
    echo '--- Expecting verification success'
    echo '--------------------------------------------------------------------'
    if nidhuggc -I ${kernel_version} -std=gnu99 $* -- ${mem_model} \
	     --extfun-no-race=fprintf --print-progress-estimate \
	     --disable-mutex-init-requirement --unroll=${unroll} ${test_file}
    then
	:
    else
	echo '^^^ Unexpected verification failure'
	failure=1
    fi
}

# Synchronization issues for rcu_process_gp_end()
runfailure v2.6.31.1 --sc gp_end_bug.c
runfailure v2.6.32.1 --sc gp_end_bug.c
runsuccess v3.0 --sc gp_end_bug.c -DKERNEL_VERSION_3

# Alleged bug between grace-period forcing and initialization
runsuccess v2.6.31.1 --tso init_bug.c -DCONFIG_NR_CPUS=3 \
	   -DCONFIG_RCU_FANOUT=2 -DFQS_NO_BUG

# Publish-Subscribe guarantee for RCU tree
runsuccess v3.19 --tso publish.c
runsuccess v3.19 --tso publish.c -DORDERING_BUG
runsuccess v3.19 --power publish.c -DPOWERPC
runfailure v3.19 --power publish.c -DPOWERPC -DORDERING_BUG

# Grace-Period guarantee -- RCU tree litmus test
runsuccess v3.19 --sc litmus.c
runfailure v3.19 --sc litmus.c -DASSERT_0
runfailure v3.19 --sc litmus.c -DFORCE_FAILURE_1
runfailure v3.19 --sc litmus.c -DFORCE_FAILURE_2
runfailure v3.19 --sc litmus.c -DFORCE_FAILURE_3
runfailure v3.19 --sc litmus.c -DFORCE_FAILURE_4
runfailure v3.19 --sc litmus.c -DFORCE_FAILURE_5
unroll=19
runfailure v3.19 --sc litmus.c -DFORCE_FAILURE_6
unroll=5
runsuccess v3.19 --sc litmus.c -DLIVENESS_CHECK_1 -DASSERT_0
runsuccess v3.19 --sc litmus.c -DLIVENESS_CHECK_2 -DASSERT_0
runsuccess v3.19 --sc litmus.c -DLIVENESS_CHECK_3 -DASSERT_0

# TSO
runsuccess v3.19 --tso litmus.c
runfailure v3.19 --tso litmus.c -DASSERT_0
runfailure v3.19 --tso litmus.c -DFORCE_FAILURE_1
runfailure v3.19 --tso litmus.c -DFORCE_FAILURE_2
runfailure v3.19 --tso litmus.c -DFORCE_FAILURE_3
runfailure v3.19 --tso litmus.c -DFORCE_FAILURE_4
runfailure v3.19 --tso litmus.c -DFORCE_FAILURE_5
unroll=19
runfailure v3.19 --tso litmus.c -DFORCE_FAILURE_6
unroll=5
runsuccess v3.19 --tso litmus.c -DLIVENESS_CHECK_1 -DASSERT_0
runsuccess v3.19 --tso litmus.c -DLIVENESS_CHECK_2 -DASSERT_0
runsuccess v3.19 --tso litmus.c -DLIVENESS_CHECK_3 -DASSERT_0


if test -n "$failure"
then
    echo '--------------------------------------------------------------------'
    echo '--- ' UNEXPECTED VERIFICATION RESULTS
    echo '--------------------------------------------------------------------'
    exit 1
else
    echo '--------------------------------------------------------------------'
    echo '--- ' Verification proceeded as expected
    echo '--------------------------------------------------------------------'
    exit 0
fi
