require File.join(File.dirname(__FILE__), 'system_control')

module System
  module Power
    module_function

    # While this method processes given block, the system does not sleep.
    #
    # @param [block] block
    #   The block that processing needs non-sleep.
    #
    # @example
    #   System::Power.no_sleep {
    #      # Add the work you need to do without
    #      # the system sleeping here.
    #   }
    def no_sleep(&block)
      if !block_given?
        raise ArgumentError, "need block"
      end

      begin
        id = no_sleep_open()
        block.call
      ensure
        no_sleep_close(id)
      end
    end

    # Gets the battery informataions.
    #
    # @returns [Hash]
    #   battery informataions
    #
    def battery_info
      battery = self.battery
      key = battery.keys
      return nil if key.count <= 0
      battery[key.first]
    end

    # Is AC Power connected?
    #
    # @returns [Bool]
    #   true  : if AC Power connected
    #   false : other
    #
    def is_AC?
      battery = self.battery_info
      return false if battery.nil?
      return false if battery["Power Source State"] != "AC Power"
      return true
    end
  end
end
