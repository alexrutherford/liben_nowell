# Summary

This code takes mainland US population data on 1km^2 grid and calculates the Liben-Nowell rank-based probability distribution ([paper here](http://www.pnas.org/content/102/33/11623.abstract)). This is extremely time consuming (essentially N^2 where N~7M) but embarrasingly parallel so therefore can be broken into chunks and processed using in parallel. This distribution is used as input into calculations described in [Limits of Social Mobilisation, Rutherford et al](http://www.pnas.org/content/110/16/6281.full)

Second set of scripts (```put\_file\_in\_db\_2.cc``` and ```put\_files\_in\_db.py```) loads distribution into a SQL database using C++ connector.

# Details

The resulting set of neighbourhoods are essentially a massive joint probability distribution of the form 'P\_ij is the probability of the friend of a recruit in cell i being in cell j'. The probability of friendship is rank based, meaning it is based on the population *lying closer to i than j*. Therefore the cumulative population centered on each cell must be calculated, although a cut-off of 1000km and 100,000 people is applied (as per Liben-Nowell et all). Much of the computational complexity arises from the fact that the data, although equally spaced, cannot easily be assigned coordinates on a grid due to projection issues. Therefore all other cells must be considered.

These files are then put in an SQL database to be retrieved as needed during simulations. Ironically, populous cells have LN neighbourhoods which are smaller (due to rank cut-off) yet are queried more frequently. Therefore cells with a small neighbourhood are stored in memory during simulations to save overhead in retrieving the same data many times.

Some guidance on using a C++/SQL connector can be found here http://blog.trilabs.co.tz/2011/09/connect-to-mysql-using-c-on-ubuntu.html

# Usage

```put\_file\_in\_db.cc``` uses SQL connector to take individual file and populate database. ```put\_files\_in\_db.py``` is wrapper to invoke this.

# Dependencies

* C++ SQL connector

# Compilation

* compiles with g++ $(mysql_config --cflags) put_file_in_db_2.cc $(mysql_config --libs)
