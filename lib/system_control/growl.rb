module System
  class Growl
    framework 'Cocoa'
    framework 'Foundation'
    require 'singleton'
    include Singleton

    GROWL_IS_READY = "Lend Me Some Sugar; I Am Your Neighbor!"
    GROWL_NOTIFICATION_CLICKED = "GrowlClicked!"
    GROWL_NOTIFICATION_TIMED_OUT = "GrowlTimedOut!"
    GROWL_KEY_CLICKED_CONTEXT = "ClickedContext"

    def regist(app, notifications, icon = nil)
      @application_name = app.to_s
      @application_icon = icon || NSApplication.sharedApplication.applicationIconImage
      @notifications = notifications.map!{|x| x.to_s}
      @default_notifications = notifications
      @center = NSDistributedNotificationCenter.defaultCenter
      send_registration!
    end

    def notify(notification, title, description, options = {})
      dict = {
        :ApplicationName => @application_name,
        #:ApplicationPID => pid,
        :NotificationName => notification.to_s,
        :NotificationTitle => title.to_s,
        :NotificationDescription => description.to_s,
        :NotificationPriority => options[:priority] || 0,
        :NotificationIcon => @application_icon.TIFFRepresentation,
      }
      dict[:NotificationSticky] = 1 if options[:sticky]

      @center.postNotificationName(:GrowlNotification, object:nil, userInfo:dict, deliverImmediately:false)
    end

    private
    def send_registration!
      dict = {
        :ApplicationName => @application_name,
        :ApplicationIcon => @application_icon.TIFFRepresentation,
        :AllNotifications => @notifications,
        :DefaultNotifications => @default_notifications
      }

      @center.postNotificationName(:GrowlApplicationRegistrationNotification, object:nil, userInfo:dict, deliverImmediately:true)
    end
  end
end

module Kernel
  unless(defined? g)
    def g(title, message = nil)
      if message.nil?
        message = title
        title = "MacRuby"
      end

      g = System::Growl.instance
      icon = g.instance_variable_get(:@application_icon)

      if icon.nil?
        icon_path = File.expand_path(File.join(__FILE__, '../../../resources/macruby.icns'))
        icon = NSImage.alloc.initWithContentsOfFile(icon_path)
      end

      notification = 'notification'
      g.regist("MacRuby System Control", [notification], icon)
      g.notify(notification, title, message)
    end
  end
end
