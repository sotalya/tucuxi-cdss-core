.. SPDX-License-Identifier: AGPL-3.0-or-later

.. _dosage:

*********************
Query dosage
*********************

.. contents:: Contents
	:depth: 1
	:backlinks: top
	:local:


A dosage history is a list of dosage time ranges. Each range has a start and
end date, and is composed of a dosage.
The dosage subsystem is extremely flexible, and allows to describe dosages such
that "take a drug at 8:00 every day, except on Sunday".

This flexibility comes with a quite complex management of the XML structure.

.. warning::

	For each specific dosage where a formulation and route has to be given,
	this formulation and route shall be identical to the one proposed by the
	selected drug model.


The dosage of a dosage time range is described inside the <dosage> tag.
A standard dosage looks like this:

.. code-block:: xml
    :linenos:

    <dosage>
      <dosageSteadyState>
        <lastDoseDate></lastDoseDate>
        <DOSAGE_CHOICE></DOSAGE_CHOICE>
      </dosageSteadyState>
      or
      <dosageLoop>
        <DOSAGE_CHOICE></DOSAGE_CHOICE>
      </dosageLoop>
      or
      <singleDoseAtTimeList>
        <singleDoseAtTime></singleDoseAtTime>
      </singleDoseAtTimeList>
      or
      <simpleDoseList>
        <unit></unit>
        <formulationAndRoute></formulationAndRoute>
        <doseList></doseList>
      </simpleDoseList>
      or
      <DOSAGE_CHOICE></DOSAGE_CHOICE>
    </dosage>


.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<dosageSteadyState>",		"",	"0:1",	"Steady state (without a starting time)"
	"<dosageLoop>",			"",	"0:1",	"Dosage treatment without time limit"
	"<singleDoseAtTimeList>",	"",	"0:1",	"List of individual doses"
	"<simpleDoseList>",		"",	"0:1",	"List of individual doses (compact representation)"
	"<DOSAGE_CHOICE>",		"",	"0:1",	"This tag does not exist --- see warning below"


A dosage contains exactly one of the proposed tags at the same time.

* ``<dosageSteadyState>`` represents a dosage that has already reached steady
  state. It acts like a ``<dosageLoop>``, but requires the date of a specific
  past dose.
* ``<dosageLoop>`` is used when the dosage is repeated continually without a
  time limit.
* ``<singleDoseAtTimeList>`` and ``<simpleDoseList>`` allow individual doses
  administered at arbitrary times.
* ``<DOSAGE_CHOICE>`` is a placeholder for a dosage with a finite duration;
  see the warning below.

.. warning:: The tag <DOSAGE_CHOICE> only exists for the representation above.
             It must be replaced by one of the following:
             ``dosageRepeat``, ``dosageSequence``, ``lastingDosage``,
             ``dailyDosage``, or ``weeklyDosage``.


DosageSteadyState
_________________

A DosageSteadyState represents a dosage at steady state. It is defined by a
last dose (that can be anywhere in time) and a sub-dosage that has been
repeated to reach the steady state.

.. code-block:: xml
	:linenos:

	<dosageSteadyState>
	  <lastDoseDate>2018-07-07T20:00:00</lastDoseDate>
	  <DOSAGE_CHOICE></DOSAGE_CHOICE>
	</dosageSteadyState>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<dosageSteadyState>",	"",			"1:1",	"Steady state dosage specifications"
	"__<lastDoseDate>",	"xs:dateTime",	"1:1",	"Date and time of a past dose"
	"__<DOSAGE_CHOICE>",	"",			"1:1",	"This tag does not exist --- see warning above"

See Dosage warning for <DOSAGE_CHOICE> tag.


DosageLoop
__________

A DosageLoop represents a dosage repeated indefinitely. It wraps a single
bounded dosage (a DOSAGE_CHOICE) that is cycled without a time limit.

.. code-block:: xml
	:linenos:

	<dosageLoop>
	  <DOSAGE_CHOICE></DOSAGE_CHOICE>
	</dosageLoop>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<dosageLoop>",		"",	"1:1",	"Loop dosage specifications"
	"__<DOSAGE_CHOICE>",	"",	"1:1",	"This tag does not exist --- see warning above"

See Dosage warning for <DOSAGE_CHOICE> tag.


DosageRepeat
____________

A DosageRepeat represents a repeated dosage. This is used when the dosage must
be taken a defined number of times.

.. code-block:: xml
	:linenos:

	<dosageRepeat>
	  <iterations>4</iterations>
	  <DOSAGE_CHOICE></DOSAGE_CHOICE>
	</dosageRepeat>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<dosageRepeat>",		"",		"1:1",	"Dosage repeat specifications"
	"__<iterations>",		"int",	"1:1",	"Number of repetitions (minimum 1)"
	"__<DOSAGE_CHOICE>",	"",		"1:1",	"This tag does not exist --- see warning above"

* The ``<iterations>`` tag must be 1 at minimum.

See Dosage warning for <DOSAGE_CHOICE> tag.


DosageSequence
______________

A DosageSequence is composed of a series of bounded dosages executed in order.

.. code-block:: xml
	:linenos:

	<dosageSequence>
	  <DOSAGE_CHOICE></DOSAGE_CHOICE>
	  <DOSAGE_CHOICE></DOSAGE_CHOICE>
	</dosageSequence>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<dosageSequence>",		"",	"1:1",	"Dosage sequence specifications"
	"__<DOSAGE_CHOICE>",	"",	"1:N",	"This tag does not exist --- see warning above"

See Dosage warning for <DOSAGE_CHOICE> tag.


LastingDosage
_____________

This type of dosage provides a dose and an interval. It is typically used
within a ``<dosageLoop>`` or a ``<dosageRepeat>``.

.. code-block:: xml
	:linenos:

	<lastingDosage>
	  <interval>24:00:00</interval>
	  <dose>
	    <value>550</value>
	    <unit>mg</unit>
	    <infusionTimeInMinutes>600</infusionTimeInMinutes>
	  </dose>
	  <formulationAndRoute>
	    <formulation>parenteralSolution</formulation>
	    <administrationName>administrationName</administrationName>
	    <administrationRoute>intramuscular</administrationRoute>
	    <absorptionModel>intravascular</absorptionModel>
	  </formulationAndRoute>
	</lastingDosage>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<lastingDosage>",				"",			"1:1",	"Lasting dosage specifications"
	"__<interval>",					"time",		"1:1",	"Dosage interval"
	"__<dose>",						"",			"1:1",	"Description of a dose"
	"____<value>",					"decimal",	"1:1",	"Dose's value"
	"____<unit>",					"string",	"1:1",	"Dose's unit"
	"____<infusionTimeInMinutes>",	"decimal",	"0:1",	"Dose's infusion time (min)"
	"__<formulationAndRoute>",		"",			"1:1",	"Description of a formulation and route"
	"____<formulation>",			"string",	"1:1",	"Formulation"
	"____<administrationName>",		"string",	"1:1",	"Name of administration"
	"____<administrationRoute>",	"string",	"1:1",	"Way of administration"
	"____<absorptionModel>",		"string",	"1:1",	"Absorption model"

* The ``<formulation>`` tag is a string enumerate. It can be "undefined",
  "parenteralSolution" or "oralSolution".
* The ``<administrationRoute>`` tag is a string enumerate. It can be
  "undefined", "intramuscular", "intravenousBolus", "intravenousDrip",
  "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal"
  or "vaginal".
* The ``<absorptionModel>`` tag is a string enumerate. It can be "undefined",
  "intravascular", "extravascular", "extravascularLag" or "infusion".


DailyDosage
___________

This type of dosage provides a dose and a time of day. It is used for a drug
taken at the same time every day (e.g. every day at 08:00).

.. code-block:: xml
	:linenos:

	<dailyDosage>
	  <time>10:00:00</time>
	  <dose>
	    <value>550</value>
	    <unit>mg</unit>
	    <infusionTimeInMinutes>600</infusionTimeInMinutes>
	  </dose>
	  <formulationAndRoute>
	    <formulation>parenteralSolution</formulation>
	    <administrationName>something</administrationName>
	    <administrationRoute>transdermal</administrationRoute>
	    <absorptionModel>infusion</absorptionModel>
	  </formulationAndRoute>
	</dailyDosage>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<dailyDosage>",				"",			"1:1",	"Daily dosage specifications"
	"__<time>",						"time",		"1:1",	"Hour of dosage"
	"__<dose>",						"",			"1:1",	"Description of a dose"
	"____<value>",					"decimal",	"1:1",	"Dose's value"
	"____<unit>",					"string",	"1:1",	"Dose's unit"
	"____<infusionTimeInMinutes>",	"decimal",	"0:1",	"Dose's infusion time (min)"
	"__<formulationAndRoute>",		"",			"1:1",	"Description of a formulation and route"
	"____<formulation>",			"string",	"1:1",	"Formulation"
	"____<administrationName>",		"string",	"1:1",	"Administration name"
	"____<administrationRoute>",	"string",	"1:1",	"Way of administration"
	"____<absorptionModel>",		"string",	"1:1",	"Absorption model"

* The ``<formulation>`` tag is a string enumerate. It can be "undefined",
  "parenteralSolution" or "oralSolution".
* The ``<administrationRoute>`` tag is a string enumerate. It can be
  "undefined", "intramuscular", "intravenousBolus", "intravenousDrip",
  "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal"
  or "vaginal".
* The ``<absorptionModel>`` tag is a string enumerate. It can be "undefined",
  "intravascular", "extravascular", "extravascularLag" or "infusion".


WeeklyDosage
_____________

This type of dosage provides a dose, a day of the week and a time of day. It
is used for a drug taken once a week.

.. code-block:: xml
	:linenos:

	<weeklyDosage>
	  <day>2</day>
	  <time>11:00:00</time>
	  <dose>
	    <value>550</value>
	    <unit>mg</unit>
	    <infusionTimeInMinutes>600</infusionTimeInMinutes>
	  </dose>
	  <formulationAndRoute>
	    <formulation>parenteralSolution</formulation>
	    <administrationName>someName</administrationName>
	    <administrationRoute>transdermal</administrationRoute>
	    <absorptionModel>infusion</absorptionModel>
	  </formulationAndRoute>
	</weeklyDosage>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<weeklyDosage>",				"",			"1:1",	"Weekly dosage specifications"
	"__<day>",						"int",		"1:1",	"Day of dosage (0 = Monday … 6 = Sunday)"
	"__<time>",						"time",		"1:1",	"Hour of dosage"
	"__<dose>",						"",			"1:1",	"Description of a dose"
	"____<value>",					"decimal",	"1:1",	"Dose's value"
	"____<unit>",					"string",	"1:1",	"Dose's unit"
	"____<infusionTimeInMinutes>",	"decimal",	"0:1",	"Dose's infusion time (min)"
	"__<formulationAndRoute>",		"",			"1:1",	"Description of a formulation and route"
	"____<formulation>",			"string",	"1:1",	"Formulation"
	"____<administrationName>",		"string",	"1:1",	"Administration name"
	"____<administrationRoute>",	"string",	"1:1",	"Way of administration"
	"____<absorptionModel>",		"string",	"1:1",	"Absorption model"

* The ``<day>`` tag must be an integer between 0 and 6 included.
* The ``<formulation>`` tag is a string enumerate. It can be "undefined",
  "parenteralSolution" or "oralSolution".
* The ``<administrationRoute>`` tag is a string enumerate. It can be
  "undefined", "intramuscular", "intravenousBolus", "intravenousDrip",
  "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal"
  or "vaginal".
* The ``<absorptionModel>`` tag is a string enumerate. It can be "undefined",
  "intravascular", "extravascular", "extravascularLag" or "infusion".


singleDoseAtTimeList
____________________

This type of dosage represents a list of individual, independent doses
administered at arbitrary times. Each entry carries its own formulation, route
and unit, allowing heterogeneous administrations within the same list.
Duplicate timestamps are rejected.

.. code-block:: xml
	:linenos:

	<singleDoseAtTimeList>
	  <singleDoseAtTime>
	    <doseDate>2021-09-12T00:36:30</doseDate>
	    <dose>
	      <value>10.0</value>
	      <unit>mg</unit>
	      <infusionTimeInMinutes>600</infusionTimeInMinutes>
	    </dose>
	    <formulationAndRoute>
	      <formulation>oralSolution</formulation>
	      <administrationName>Eliquis° oral tablet</administrationName>
	      <administrationRoute>oral</administrationRoute>
	      <absorptionModel>extravascular</absorptionModel>
	    </formulationAndRoute>
	  </singleDoseAtTime>
	  <singleDoseAtTime>
	    <doseDate>2021-09-12T10:36:30</doseDate>
	    <dose>
	      <value>20.0</value>
	      <unit>mg</unit>
	      <infusionTimeInMinutes>600</infusionTimeInMinutes>
	    </dose>
	    <formulationAndRoute>
	      <formulation>oralSolution</formulation>
	      <administrationName>Another oral tablet</administrationName>
	      <administrationRoute>oral</administrationRoute>
	      <absorptionModel>extravascular</absorptionModel>
	    </formulationAndRoute>
	  </singleDoseAtTime>
	</singleDoseAtTimeList>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<singleDoseAtTimeList>",			"",			"1:1",	"List of individual dosages"
	"__<singleDoseAtTime>",			"",			"1:N",	"Individual dosage specifications"
	"____<doseDate>",				"date-time",	"1:1",	"Date and time of administration"
	"____<dose>",					"",			"1:1",	"Description of a dose"
	"______<value>",				"decimal",	"1:1",	"Dose's value"
	"______<unit>",					"string",	"1:1",	"Dose's unit"
	"______<infusionTimeInMinutes>",	"decimal",	"0:1",	"Dose's infusion time (min)"
	"____<formulationAndRoute>",		"",			"1:1",	"Description of a formulation and route"
	"______<formulation>",			"string",	"1:1",	"Formulation"
	"______<administrationName>",	"string",	"1:1",	"Administration name"
	"______<administrationRoute>",	"string",	"1:1",	"Way of administration"
	"______<absorptionModel>",		"string",	"1:1",	"Absorption model"

* The ``<formulation>`` tag is a string enumerate. It can be "undefined",
  "parenteralSolution" or "oralSolution".
* The ``<administrationRoute>`` tag is a string enumerate. It can be
  "undefined", "intramuscular", "intravenousBolus", "intravenousDrip",
  "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal"
  or "vaginal".
* The ``<absorptionModel>`` tag is a string enumerate. It can be "undefined",
  "intravascular", "extravascular", "extravascularLag" or "infusion".


simpleDoseList
______________

Similarly to ``<singleDoseAtTimeList>``, this type of dosage represents
individual doses administered at arbitrary times.
By sharing the unit of measurement and the formulation and route across all
entries, it provides a more compact representation.
Duplicate timestamps are rejected.

.. code-block:: xml
	:linenos:

	<simpleDoseList>
	  <unit>mg</unit>
	  <formulationAndRoute>
	    <formulation>oralSolution</formulation>
	    <administrationName>Eliquis° oral tablet</administrationName>
	    <administrationRoute>oral</administrationRoute>
	    <absorptionModel>extravascular</absorptionModel>
	  </formulationAndRoute>
	  <doseList>
	    <doseDateValue>
	      <doseDate>2021-09-12T00:36:30</doseDate>
	      <value>10.0</value>
	      <infusionTimeInMinutes>600</infusionTimeInMinutes>
	    </doseDateValue>
	    <doseDateValue>
	      <doseDate>2021-09-12T01:36:30</doseDate>
	      <value>20.0</value>
	      <infusionTimeInMinutes>600</infusionTimeInMinutes>
	    </doseDateValue>
	  </doseList>
	</simpleDoseList>

.. csv-table::
	:header: "Tag name", "Format", "Occ.", "Description"
	:widths: 20, 15, 15, 30

	"<simpleDoseList>",				"",			"1:1",	"Compact list of individual dosages"
	"__<unit>",						"string",	"1:1",	"Common unit of measurement for all doses"
	"__<formulationAndRoute>",		"",			"1:1",	"Common formulation and route for all doses"
	"____<formulation>",			"string",	"1:1",	"Formulation"
	"____<administrationName>",		"string",	"1:1",	"Administration name"
	"____<administrationRoute>",	"string",	"1:1",	"Way of administration"
	"____<absorptionModel>",		"string",	"1:1",	"Absorption model"
	"__<doseList>",					"",			"1:1",	"List of doses"
	"____<doseDateValue>",			"",			"1:N",	"Individual dose entry"
	"______<doseDate>",				"date-time",	"1:1",	"Date and time of administration"
	"______<value>",				"decimal",	"1:1",	"Dose's value"
	"______<infusionTimeInMinutes>",	"decimal",	"0:1",	"Dose's infusion time (min)"

* The ``<formulation>`` tag is a string enumerate. It can be "undefined",
  "parenteralSolution" or "oralSolution".
* The ``<administrationRoute>`` tag is a string enumerate. It can be
  "undefined", "intramuscular", "intravenousBolus", "intravenousDrip",
  "nasal", "oral", "rectal", "subcutaneous", "sublingual", "transdermal"
  or "vaginal".
* The ``<absorptionModel>`` tag is a string enumerate. It can be "undefined",
  "intravascular", "extravascular", "extravascularLag" or "infusion".
