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
  end
end
