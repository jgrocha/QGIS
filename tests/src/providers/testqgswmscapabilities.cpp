/***************************************************************************
    testqgswmscapabilities.cpp
    ---------------------
    begin                : May 2016
    copyright            : (C) 2016 by Patrick Valsecchi
    email                : patrick dot valsecchi at camptocamp dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QFile>
#include <QObject>
#include "qgstest.h"
#include "qgssettings.h"

#include <qgswmscapabilities.h>
#include <qgsapplication.h>
#include <qgswmsconnection.h>
#include <qgswmsprovider.h>

/**
 * \ingroup UnitTests
 * This is a unit test for the WMS capabilities parser.
 */
class TestQgsWmsCapabilities: public QObject
{
    Q_OBJECT
  private slots:

    void initTestCase()
    {
      // init QGIS's paths - true means that all path will be inited from prefix
      QgsApplication::init();
      QgsApplication::initQgis();
    }

    //runs after all tests
    void cleanupTestCase()
    {
      QgsApplication::exitQgis();
    }

    void addConnections()
    {
      // split the initialization
      // split in two different tests, one for the filesystem and another for the Postgresql stored project

      QMap<QString, QString> exampleServers;
      exampleServers[QStringLiteral( "QGIS Server project from file sytem" )] = QStringLiteral( "http://qgis.demo/cgi-bin/qgis_mapserv.fcgi?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities&MAP=/home/qgis/projects/cartageologica.qgz" );
      exampleServers[QStringLiteral( "QGIS Server project from Postgresql" )] = QStringLiteral( "http://qgis.demo/cgi-bin/qgis_mapserv.fcgi?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities&MAP=postgresql%3A%3Fservice%3Dpg_geotuga%26sslmode%3Ddisable%26dbname%3D%26schema%3Dortos%26project%3Dteste" );
      QgsSettings settings;
      settings.beginGroup( QStringLiteral( "qgis/connections-wms" ) );

      QMap<QString, QString>::const_iterator i = exampleServers.constBegin();
      for ( ; i != exampleServers.constEnd(); ++i )
      {
        // Only do a server if it's name doesn't already exist.
        QStringList keys = settings.childGroups();
        if ( !keys.contains( i.key() ) )
        {
          QString path = i.key();
          settings.setValue( path + "/url", i.value() );
        }
      }
      settings.endGroup();

      QgsWMSConnection connection1( QStringLiteral( "QGIS Server project from file sytem" ) );
      QgsWMSConnection connection2( QStringLiteral( "QGIS Server project from Postgresql" ) );
      QgsDataSourceUri mUri1 = connection1.uri();
      QgsDataSourceUri mUri2 = connection2.uri();

      QgsWmsSettings wmsSettings1, wmsSettings2;
      bool parse1 = wmsSettings1.parseUri( mUri1.encodedUri() );
      bool parse2 = wmsSettings2.parseUri( mUri2.encodedUri() );

      QCOMPARE( parse1, 1 );
      QCOMPARE( parse2, 1 );

      QCOMPARE( wmsSettings1.baseUrl(), QStringLiteral( "http://qgis.demo/cgi-bin/qgis_mapserv.fcgi?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities&MAP=/home/qgis/projects/cartageologica.qgz&" ) );
      QCOMPARE( wmsSettings2.baseUrl(), QStringLiteral( "http://qgis.demo/cgi-bin/qgis_mapserv.fcgi?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities&MAP=postgresql%3A%3Fservice%3Dpg_geotuga%26sslmode%3Ddisable%26dbname%3D%26schema%3Dortos%26project%3Dteste&" ) );

      // QgsWmsCapabilitiesDownload capDownload( QgsWmsProvider::prepareUri( mUri2.param( "url" ), false ), wmsSettings2.authorization(), true );

    }


    void read()
    {
      QgsWmsCapabilities capabilities;

      QFile file( QStringLiteral( TEST_DATA_DIR ) + "/provider/GetCapabilities.xml" );
      QVERIFY( file.open( QIODevice::ReadOnly | QIODevice::Text ) );
      const QByteArray content = file.readAll();
      QVERIFY( content.size() > 0 );
      const QgsWmsParserSettings config;

      QVERIFY( capabilities.parseResponse( content, config ) );
      QCOMPARE( capabilities.supportedLayers().size(), 5 );
      QCOMPARE( capabilities.supportedLayers()[0].name, QString( "agri_zones" ) );
      QCOMPARE( capabilities.supportedLayers()[1].name, QString( "buildings" ) );
      QCOMPARE( capabilities.supportedLayers()[2].name, QString( "land_surveing_parcels" ) );
      QCOMPARE( capabilities.supportedLayers()[3].name, QString( "cadastre" ) );
      QCOMPARE( capabilities.supportedLayers()[4].name, QString( "test" ) );

      // make sure the default style is not seen twice in the child layers
      QCOMPARE( capabilities.supportedLayers()[3].style.size(), 1 );
      QCOMPARE( capabilities.supportedLayers()[3].style[0].name, QString( "default" ) );
      QCOMPARE( capabilities.supportedLayers()[1].style.size(), 1 );
      QCOMPARE( capabilities.supportedLayers()[1].style[0].name, QString( "default" ) );
      QCOMPARE( capabilities.supportedLayers()[2].style.size(), 1 );
      QCOMPARE( capabilities.supportedLayers()[2].style[0].name, QString( "default" ) );

      // check it can read 2 styles for a layer and that the legend URL is OK
      QCOMPARE( capabilities.supportedLayers()[0].style.size(), 2 );
      QCOMPARE( capabilities.supportedLayers()[0].style[0].name, QString( "yt_style" ) );
      QCOMPARE( capabilities.supportedLayers()[0].style[0].legendUrl.size(), 1 );
      QCOMPARE( capabilities.supportedLayers()[0].style[0].legendUrl[0].onlineResource.xlinkHref,
                QString( "http://www.example.com/yt.png" ) );
      QCOMPARE( capabilities.supportedLayers()[0].style[1].name, QString( "fb_style" ) );
      QCOMPARE( capabilities.supportedLayers()[0].style[1].legendUrl.size(), 1 );
      QCOMPARE( capabilities.supportedLayers()[0].style[1].legendUrl[0].onlineResource.xlinkHref,
                QString( "http://www.example.com/fb.png" ) );
    }

};

QGSTEST_MAIN( TestQgsWmsCapabilities )
#include "testqgswmscapabilities.moc"
