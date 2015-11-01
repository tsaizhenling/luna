require 'CSV'
require 'cgi'
require 'lemmatizer'
require 'httpclient'
require 'json'
require 'erb'
require 'colorize'
require 'thor'

# constants
PREVIEW_FILE_NAME = "preview.html"
SKUS_INPUT_FILE_NAME = ARGV[0]
BOW_FILE_NAME = "bag-of-words.txt"
KEYWORD_OUPUT_FILE_NAME = "keywords.txt"
SEGMENT = ARGV[1] || "women"

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
	file = File.new(BOW_FILE_NAME,"w")
	# remove if no bag of words
	productArray.reject! { |product| product[:bow].count == 0 }
	# output processed data
	productArray.map do |product| 
		product[:bow].map do |word|
			file << word << " "
		end
		file << "\n"
	end
	file.close
	system("./bin/ap2 #{BOW_FILE_NAME} #{KEYWORD_OUPUT_FILE_NAME}")
	file = File.new(KEYWORD_OUPUT_FILE_NAME, "r")
	keywords = Array.new
	file.each_line do |line|
		keywords.push(line)
	end
	file.close
	return keywords
end

def get_products(queries,productArray)
	if queries.count == 0
		puts "sorry i don't know what to reccommend".red
		exit()	
	end

	puts "---------------------------------------------------------".blue
	puts "--------------getting products from api------------------".blue
	puts "---------------------------------------------------------".blue

	# make queries for products
	http = HTTPClient.new
	url = "https://api.zalora.sg/v1/products"
	results = Array.new
	queries.each do |query|
		response = http.get(url,{"query"=>query,"segment"=>SEGMENT},{ 'Accept' => 'application/json' })
		if response.status == 200
			productList = JSON.parse(response.body)
			array = productList["data"]["products"]
			if array
				array.delete_at(0)
				array.delete_at(0)
			
				if results.count == 0
		 			results = array
				else
			 		results = results.zip(array).flatten.compact
				end 
				puts "got #{productList["data"]["product_count"]} products for " + query
			else 
				puts "got no products for " + query
			end
		else
			puts "get products for #{query} failed"
		end
	end

	# remove duplicate products
	results.uniq!
	# remove products already viewed
	skusViewed = productArray.map { |product| product[:sku] }
	results.select! do |product|
		!skusViewed.include?product["config_sku"]
	end
	return results
end

def template
		%{
	        <DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
	
	    <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	    <head>
	            <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
	            <title>Reccommended products</title>
				<style>
	 			#container div {
	 			 display: inline-block;
	 			 margin: 0 1em 0 1em;
	 			 width: 15%;
				}
				</style>
	        </head>
	        <body>
	        <div id="container">
	               <% for product in results %>
	               <div>
	                  <img src="<%= product["main_image_url"] %>" width="150" height="200"></img>
	                  <p><b><%= product["brand"] %></b></p>
	                  <p><%= product["name"] %></p>
	               </div>
	               <% end %>
	        </div>
	        </body>
	        </html>
	  }
	end

class Cli < Thor
	desc(
    'reccomend <input filepath> <segment>(default to women) <option:preview>(default to true)','tries to reccomend products based on user product view history'
    )
    option :preview, :type => :boolean, :default => true
    def reccomend(inputFilePath,segment='women')
    	productArray = parse_data(inputFilePath)
    	queries = get_keywords(productArray)
    	results = get_products(queries,productArray)
    	# preview everything
		data = ERB.new(template).result(binding)
		file = File.open(PREVIEW_FILE_NAME, "w")
		file.write(data)
		file.close
		if options['preview']
			system("open -a \"Google Chrome\" #{PREVIEW_FILE_NAME}")
		else 
			puts "no preview"
		end
    end
end

Cli.start

