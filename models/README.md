# Models

This folder stores json files of robot models.

The data structure of the robot model has yet been decided.

A suggestion is describing every degree of freedom, including:

* Every mass

    * mass_id

    * position Vec (e.g. 0,0,0, the origin)

    * damping (e.g. 0.9996, every move will be decreased by a factor of 0.9996)

* Every spring

    * left end: mass_id

    * right end: mass_id

    * _type: (e.g. 0, defined as ACTIVE_CONTRACT_THEN_EXPAND)

    * _omega: (e.g. 4, contract 4 times per second)