require 'CSV'
require 'cgi'
require 'lemmatizer'
require 'httpclient'
require 'json'
require 'erb'
require 'colorize'
require 'thor'
require './bin/luna.bundle'

# constants
SKUS_INPUT_FILE_NAME = ARGV[0]
BOW_FILE_NAME = "bag-of-words.txt"
KEYWORD_OUPUT_FILE_NAME = "keywords.txt"

def parse_data(inputFilePath)
	file = File.new(inputFilePath, "r")
    productArray = []
    file.each_line do |line|
        product = {}
        product[:tokens] = line.split(" ")
        productArray.push(product)
    end
	file.close
	# lemmatize words
	lem = Lemmatizer.new
	productArray.map do |product|
        productNameTokens = product[:tokens]
        product[:bow] = []
		productNameTokens.map do |string|
            product[:bow] += [lem.lemma(string)]
		end
		product[:bow].uniq!
	end
	return productArray
end

def get_keywords(productArray)
	productArray.reject! { |product| product[:bow].count == 0 }
	products = Luna::StringSetVector.new
	productArray.map do |product| 
		words = Luna::StringSet.new
		product[:bow].map do |word|
			words << word
		end
		products << words
	end
	classifier = Luna::Classifier.new(products)
	keywords = classifier.deriveKeywords()
	queries = []
	keywords.map do |words|
		query = ""
		words.map do |word|
			query = query + word + " "
		end
		queries << query
	end
	return queries
end

class Cli < Thor
	desc(
    'reccomend <input filepath> <option:preview>(default to true)','tries to reccomend products based on user product view history'
    )
    option :preview, :type => :boolean, :default => true
    def reccomend(inputFilePath)
    	productArray = parse_data(inputFilePath)
    	queries = get_keywords(productArray)
    	for query in queries
    		if options['preview']
			system("open -a \"Google Chrome\" \'http://www.amazon.com/s/ref=nb_sb_noss_2?url=search-alias%3Dfashion&field-keywords="+query+"\'")
		else 
			puts "no preview for keywords : " + query
		end
    	end
		
    end
end

Cli.start

