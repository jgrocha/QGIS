#!/usr/bin/env bash
###########################################################################
#    before_install.sh
#    ---------------------
#    Date                 : March 2019
#    Copyright            : (C) 2019 by Denis Rouzaud
#    Email                : denis@opengis.ch
###########################################################################
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
###########################################################################

wget https://launchpad.net/ubuntu/+archive/primary/+files/libseccomp2_2.4.1-0ubuntu0.16.04.2_amd64.deb
sudo dpkg -i libseccomp2_2.4.1-0ubuntu0.16.04.2_amd64.deb
sudo apt-get install -f
