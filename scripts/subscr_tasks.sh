YEARDATE="$(python gettodaystring.py)"
echo "moving ../mcsimrunner_ng/new_signups.csv to ../useradmin/new_${YEARDATE}.csv ..."
mv ../mcsimrunner_ng/new_signups.csv ../useradmin/new_"${YEARDATE}".csv
echo "sending email to admin with instructions ..."
mailx -s "mcweb subscriptions: new_${YEARDATE}.csv" jaga@fysik.dtu.dk < subscr_email
