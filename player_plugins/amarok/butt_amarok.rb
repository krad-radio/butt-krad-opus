#!/usr/bin/env ruby
#
# Now playing script for IRC.

# Use with the "/exec -o" command of your client. You can bind an alias like this:
# /alias np exec -o /home/myself/amaroknowplaying.rb
#
# (c) 2005-2006 Mark Kretschmann <markey@web.de>
# License: GNU General Public License V2
#
# modified by Daniel Nöthen for use in combination with butt


loop do
    message = gets().chomp() #Read message from stdin
    command = /[A-Za-z]*/.match( message ).to_s()

    case command
        when "configure"
            msg  = '"This script does not have configuration options."'
            `dcop amarok playlist popupMessage "#{msg}"`

        when "trackChange"

            title  = `dcop amarok player title 2> /dev/null`.chomp
            exit( 1 ) unless $?.success? # Abort if Amarok isn't running
            artist = `dcop amarok player artist`.chomp
            album  = `dcop amarok player album`.chomp
            year   = `dcop amarok player year`.chomp
            lastfm = `dcop amarok player lastfmStation`.chomp

            output = ""

            if title.empty?
                output += `dcop amarok player nowPlaying`.chomp
            else
                # Strip file extension
                extensions = ".ogg", ".mp3", ".wav", ".flac", ".fla", ".wma", ".mpc"
                ext = File.extname( title ).downcase

                if extensions.include?( ext )
                    title = title[0, title.length - ext.length]
                end

                if artist.empty?
                    output += "#{title}"
                else
                    output += "#{artist} - #{title}"
                end

                unless album.empty?
                    output += " [#{album}"
                    output += ", #{year}" unless year == "0"
                    output += "]"
                end

                unless lastfm.empty?
                    output += " (Last.fm #{lastfm})"
                end
            end
            f = File.open(File.expand_path("~/.amarok_song"), File::WRONLY|File::TRUNC|File::CREAT)
            f.puts "#{output}"
            f.close

    end
end
