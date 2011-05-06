#!/usr/local/bin/macruby
# -*- coding: utf-8 -*-

# Usage:
#  $ ./growl.rb

require 'rubygems'
require 'system_control'

g "Growl Example1"
g "Notification Title", "Growl Example2"
g "Notification Title", "Growl Example3", {:sticky => true}
g "Growl Example4", {:sticky => true}
