#!/bin/sh

source funcs.sh
set_md_plugin_dir

RES=/tmp/dsl-test.dummy

check() {
		../src/makedict -i dsl -o dummy "$1" > "$2"

		if ! diff -u "$2" "$3"; then
				echo "$0 failed with data: $1, see $RES" >&2
				exit 1
		fi
		rm -f $RES
}

check sample-dicts/skip_comments.dsl $RES sample-dicts/skip_comments_dsl_std.dummy 
check sample-dicts/ref_param.dsl $RES sample-dicts/ref_param_std.dummy
check sample-dicts/right_tag_order.dsl $RES sample-dicts/right_tag_order_std.dummy
check sample-dicts/col_param.dsl $RES sample-dicts/col_param_std.dummy
check sample-dicts/dsl_transc.dsl $RES sample-dicts/dsl_transc_std.dummy
check sample-dicts/opt_nu.dsl $RES sample-dicts/opt_nu_std.dummy

exit 0