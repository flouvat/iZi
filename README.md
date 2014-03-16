# iZi: Easy prototyping of pattern mining algorithms

Although guided by real problems, data mining techniques are still marginally used and their implementation can only be carried out by specialists programmers familiar with “low-level” code. The technology transfer is thus slowed down by some limitations, among which the time necessary to the development of operational programs.

We consider here an important and broad class of data mining problems: the discovery of interesting patterns in large databases. From a theoretical point of view, these problems must be representable as sets [1], i.e. the solution space is isomorphic to a boolean lattice.

In this setting, we propose a library facilitating the resolution of such problems, based on the use of generic and scalable algorithms. The characteristics and optimizations of the algorithms are transparent for the programmer, and only the development of the specific properties of its problem is left to him/her.

The library is developed in C++ and is under GNU General Public License.

[1] Heikki Mannila and Hannu Toivonen, Levelwise Search and Borders of Theories in Knowledge Discovery, Data Mining and Knowledge Discovery, volume 1, number 3, 241-258, 1997.

## Platforms supported

The library has been compiled on an unix environnement using GCC v3.4.1 and GCC v3.4.4, and on Windows using Mingw/GCC 3.4.2.

## Installation

-  Download the library.  
-  Unpack the zip file in a directory.  

To use the library, you just need to include the header files in your source code and compile your source files.

## Documentation

The technical documentation is available [here](http://liris.cnrs.fr/izi/wiki/doku.php?id=start).

## Examples of main files

The library contains some examples of main files. These files are stored in the “Examples main prog” directory of the library.

The following problems are already integrated to the library:
* The discovery of frequent itemsets in a transactional database
* The discovery of frequent essential itemsets in a transactional database
* The discovery of primary keys of a relation
* The discovery of satisfied inclusion dependencies between two relations
* The file mainFrequent-Algorithms.cpp shows an example of main program using the algorithms Apriori or ABS to solve the frequent itemsets problem.

The file mainFrequentEssential-Apriori.cpp shows an example of main program using the algorithm Apriori to extract frequent essential itemsets of a transactional database.

The file mainMinimalKey.cpp shows an example of main program using the algorithms Apriori or ABS to extract keys of a relational database.

The file mainIND.cpp shows an example of main program using the algorithms Apriori or ABS to extract satisfied inclusion dependencies between two relations.

## Citing the library

If you want to refer to our library in a publication, please cite the following publication:

Frédéric Flouvat, Fabien De Marchi et Jean-Marc Petit. The iZi project: easy prototyping of interesting pattern mining algorithms. New Frontiers in Applied Data Mining, PAKDD 2009 International Workshops, Revised Selected Papers, LNCS 5669, ©Springer-Verlag, p. 1-15, 2009. pdf

## Publications related to iZi

* Lhouari Nourine, Jean-Marc Petit. Extending Set-Based Dualization: Application to Pattern Mining. ECAI 2012, IOS Press ed. Montpellier, France. 2012.
* Frédéric Flouvat, Fabien De Marchi et Jean-Marc Petit. The iZi project: easy prototyping of interesting pattern mining algorithms. New Frontiers in Applied Data Mining, PAKDD 2009 International Workshops, Revised Selected Papers, LNCS 5669, ©Springer-Verlag, p. 1-15, 2009. pdf
* Hélène Jaudoin, Frédéric Flouvat, Jean-Marc Petit et Farouk Toumani. Towards a scalable query rewriting algorithm in presence of value constraints. Journal on Data Semantics (JoDS XII), Vol. 12, p.37-65, 2009.
* Frédéric Flouvat, Fabien De Marchi and Jean-Marc Petit. The open source library iZi for pattern mining problems, Open Source in Data Mining (OSDM) workshop, in conjonction with PAKDD'09, p. 14-25, Bangkok, Thailand, April 2009.
* Frédéric Flouvat, Fabien De Marchi and Jean-Marc Petit. iZi: a new toolkit for pattern mining problems, 17th International Symposium on Methodologies for Intelligent Systems (ISMIS), p. 131-136, Toronto, Canada, May 2008.
* Frédéric Flouvat, Fabien De Marchi and Jean-Marc Petit. Rapid prototyping of pattern mining problems isomorphic to boolean lattices, IEEE International Conference on Research Challenges in Information Science (RCIS), p. 1-11, Marrakech, Morocco, June 2008. pdf 41 long papers accepted on 108 submissions.
* Frédéric Flouvat, Fabien De Marchi and Jean-Marc Petit. Pattern mining problems isomorphic to boolean lattices: from problem statements to efficient implementations , Third Franco-Japanese Workshop on Information Search, Integration and Personalization (ISIP'07), Hokkaido, Japan, June 2007. (invited paper)
* Fabien De Marchi, Frédéric Flouvat and Jean-Marc Petit. Adaptive strategies for mining the positive border of interesting patterns: application to inclusion dependencies in databases, Constraint-based Mining and Inductive Databases, Revised Selected Papers, Jean-Francois Boulicaut, Luc De Raedt and Heikki Mannila Ed., LNCS 3848, p. 81-101, 2005. ©Springer-Verlag 2005.
* Frédéric Flouvat, Fabien De Marchi and Jean-Marc Petit, A thorough experimental study of datasets for frequent itemsets, 5th IEEE International Conference on Data Mining (ICDM'05), Houston, USA, November 2005.
* Frédéric Flouvat, Fabien De Marchi and Jean-Marc Petit, ABS: Adaptive Borders Search of frequent itemsets, 2nd Workshop on Frequent Itemsets Mining Implementation (FIMI'04), in conjonction with ICDM'04, Brighton, UK, November 2004. CEUR Workshop Proceedings, Vol. 126.
* Fabien De Marchi and Jean-Marc Petit, Zigzag: a new algorithm for mining large inclusion dependencies in database, 3rd IEEE International Conference on Data Mining (ICDM'03), Boston, USA, November 2003.
* Fabien De Marchi, Stéphane Lopes and Jean-Marc Petit, Efficient Algorithms for Mining Inclusion Dependencies, 8th International Conference on Extending Database Technology (EDBT 2002), Prague, Czech Republic, March 2002.
* Stéphane Lopes, Jean-Marc Petit and Lotfi Lakhal, Efficient Discovery of Functional Dependencies and Armstrong Relations, 7th International Conference on Extending Database Technology (EDBT 2000), Konstanz, Germany, March 2000.

## Related Projects

* Data Mining Template Library (DMTL), M.J. Zaki and al.,Computer Science Department, Rensselear Polytechnic Institute. DMTL is an open-source generic data mining library for frequent patterns (itemsets, sequences, trees and graphs) discovery.  
* DAG (2009-2013) (ANR DEFIS 2009 funding)

## iZi contributors

* Frédéric Flouvat, Associate professor, PPME, University of New Caledonia, New Caledonia.
* Jean-Marc Petit, Professor, LIRIS, Université de Lyon, INSA-Lyon, France.
* Fabien De Marchi, Associate professor, LIRIS, Université de Lyon, Université Lyon 1, France.
* Emmanuel Coquery, Associate professor, LIRIS, Université de Lyon, Université Lyon 1, France
* Prisca Bonnet, undergraduate student, INSA (L3 level)
* Florent Weillaert, undergraduate student, INSA (L3 level)

## Feedbacks

If you have any questions or suggestions concerning use and development of the library, please mail to Frédéric Flouvat.
