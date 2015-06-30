# luna

lean reccommendation engine for native mobile applications. named after Luna, advisor to sailor moon.

## poc - proof of concept

this folder contains a command line application which is a working prototype of luna.

any experiment to change the strategy of luna should be carried out on this prototype first

**dependencies**

please install the following ruby gems
```
colorize
lemmatizer
httpclient
json
thor
```

**to use**

run ```ruby poc.rb``` for user guide

**how it works**

input: list of products. this list can be dictated by user interest or user browsing history

* generate bag of words using product name and product color
* lemmatize the bag of words
* calculate similarities between each pair of product using jaccardIndex (have tried cosine and euclidean distance also)
* run affinity propagation on the dataset
* get key words that represent each cluster
* use the keys words to query mobile API like a search result
* interleave the individual search results

**limitations**

* has a dependency on mobile API search functionality and by extension, solr's product indexing
* only able to find good open-source lemmatizers for english at the moment. luna will not work so well for other languages(for now)

### sampleGenerator - helper to generate sample data for testing

to use grab a list of skus. you can get some from google analytics
please use SKUs from SG live only, as the generator gets product info from SG live

run `ruby generator.rb datain dataout`

## production (empty for now)

language of choice ==> c++
