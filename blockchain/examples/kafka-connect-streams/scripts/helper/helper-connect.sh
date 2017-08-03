#!/usr/bin/env bash

TABLE=$1
PACKAGE=$2
DB=/usr/local/lib/retail.db

echo "DROP TABLE IF EXISTS $TABLE;" | sqlite3 $DB
echo "CREATE TABLE $TABLE(id INTEGER KEY NOT NULL, name VARCHAR(255), sale INTEGER);" | sqlite3 $DB
echo ".import files/table.$TABLE $TABLE" | sqlite3 $DB

echo -e "\n======= Viewing schema of table $TABLE in the database $DB ======="
echo "pragma table_info($TABLE);" | sqlite3 $DB
echo -e "\n======= Viewing rows from table $TABLE in the database $DB ======="
echo "select * from $TABLE;" | sqlite3 $DB

sed -i "/^connection.url/c\connection.url=jdbc:sqlite:$DB" files/$PACKAGE/source-quickstart-sqlite.properties

jps | grep ConnectStandalone | awk '{print $1;}' | xargs kill -9
SLEEPTIME=30
echo -e "\n======= Running standalone Connect and sleeping $SLEEPTIME seconds before continuing ======="
nohup connect-standalone files/$PACKAGE/connect-standalone.properties files/$PACKAGE/source-quickstart-sqlite.properties </dev/null &>/dev/null &
sleep $SLEEPTIME
jps | grep ConnectStandalone | awk '{print $1;}' | xargs kill -9
