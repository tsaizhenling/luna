# luna

lean portable reccommendation engine. named after Luna, advisor to sailor moon.



luna.bundle contains all the functions required for clustering and is intended to function as the core of the reccommendation engine

see prod.rb for an example on how to pre-process and post-process the data.

libluna is built for testing

sample usage:

    ruby prod.rb reccomend sampleA.txt
    
the demo will launch a google chrome window with an amazon search result for each cluster

**Dependencies**

Use bundler to install all dependencies, to run poc.rb

    bundle install

we use [swig](http://www.swig.org/) to generate the ruby interface for luna


**Building**

    cd production
    make

**Unit tests**

first, install gtest
see https://code.google.com/p/tonatiuh/wiki/InstallingGoolgeTestForMac

    make clean
    make test

**Notes on preprocessing data**

the words passed in should be lemmatized, so that works like `pleated` and `pleat` are recognized as the same keyword.

**how it works**

input: list of products. this list can be dictated by user interest or user browsing history

*pre-processing*
* generate bag of words using product name and product color
* lemmatize the bag of words

*clustering*
* calculate similarities between each pair of product using jaccardIndex (have tried cosine and euclidean distance also)
* run affinity propagation on the dataset
* get key words that represent each cluster

*post-processing*
* use the keys words to query an API like a search result

**limitations**

* has a dependency on API search functionality and by extension, the search API's product indexing




