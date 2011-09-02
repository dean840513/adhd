# Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This file contains definitions of utilities which are used by the
# build system.  No utility should be used directly by-name in the
# makefiles; instead, each utility should have a definition here and
# the macro value should be used.
#
# This makes it easy to ensure there are no host OS utility
# dependencies when cross compiling.

MKDIR	= /bin/mkdir
AR	= /usr/bin/ar
