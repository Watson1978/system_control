#!/usr/local/bin/macruby
# -*- coding: utf-8 -*-
require 'rubygems'
require 'hotcocoa'
require 'yaml'
require 'system_control'

file = 'address.yaml'

class Wake
  include HotCocoa

  def initialize(file)
    get_addr(file)
  end

  def run
    application(:name => "Wake") do |app|
      app.delegate = self
      window(:frame => [100, 100, 250, 300], :title => "Wake") do |win|
        win.will_close { exit }

        win.view = layout_view(:layout => {:expand => [:width, :height],
                               :padding => 0, :margin => 0}) do |vert|
          vert << scroll_view(:layout => {:expand => [:width, :height]}) do |scroll|
            scroll.setAutohidesScrollers(true)
            scroll << @table = table_view(:columns => [column(:id => :name, :title => ''),
                                                       column(:id => :mac_address, :title => '')],
                                          :data => @address) do |table|
              table.setUsesAlternatingRowBackgroundColors(true)
              table.setGridStyleMask(NSTableViewSolidHorizontalGridLineMask)

            end
          end

          vert << layout_view(:frame => [0, 0, 0, 30], :mode => :horizontal,
                              :layout => {:padding => 0, :margin => 0,
                                          :start => false, :expand => [:width]}) do |horiz|
            horiz << button(:title => 'Wake', :layout => {:align => :center}) do |b|
              b.on_action { wake }
            end
          end
        end
      end
    end
  end

  def wake
    i = @table.selectedRowIndexes.firstIndex
    @address[i][:mac_address]
    begin
      System::Network.wake(@address[i][:mac_address])
    rescue => err
      p err
    end
  end

  def get_addr(file)
    @address = []
    unless(File.exist?(file))
      `arp -a`.split(/\n/).each do |line|
        if(line =~ /\((.+)\) at ([^\s\(\)]+) on/)
          @address << { :name => $1, :mac_address => $2 }
        end
      end
      File.open(file, "w") {|f|
        YAML.dump(@address, f)
      }
    else
      @address = YAML.load_file(file)
    end
  end
end

Wake.new(file).run
