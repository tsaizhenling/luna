# luna

lean portable reccommendation engine. named after Luna, advisor to sailor moon.

## production

libluna contains all the functions required for clustering and is intended to function as the core of the reccommendation engine

see prod.rb for an example on how to pre-process and post-process the data.

sample usage:

    ruby prod.rb reccomend sample2.txt

**Dependencies**

Use bundler to install all dependencies, to run poc.rb

    bundle install

**Building**

    cd production
    make no-test

**How to use**

see example/main.cpp on how to use libluna

    vector<set<string> > products;
    // populate products
    .
    .
    Classifier classifier(products);
    set<set<string> > keywords = classifier.deriveKeywords(cout);

**Unit tests**

first, install gtest
see https://code.google.com/p/tonatiuh/wiki/InstallingGoolgeTestForMac

    make clean
    make

**Notes on preprocessing data**

the words passed in should be lemmatized, so that works like `pleated` and `pleat` are recognized as the same keyword.

## poc - proof of concept

this folder contains a command line application which is a working prototype of luna.

any experiment to change the strategy of luna should be carried out on this prototype first

**to use**

run ```ruby poc.rb``` for user guide

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
* use the keys words to query mobile API like a search result
* interleave the individual search results

**limitations**

* has a dependency on API search functionality and by extension, the search API's product indexing





