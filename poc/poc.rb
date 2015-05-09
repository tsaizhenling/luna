require 'CSV'
require 'cgi'
require "lemmatizer"
require 'httpclient'
require 'json'
require 'erb'
require 'colorize'

# constantd
PREVIEW_FILE_NAME = "preview.html"
SKUS_INPUT_FILE_NAME = ARGV[0]
BOW_FILE_NAME = "bag-of-words.txt"
CLUSTERER_INPUT_FILE_NAME = "sim.txt"
CLUSTERER_OUPUT_FILE_NAME = "cluster-index.txt"

def jaccardIndex(text1, text2)
	union = text1 | text2
	intersect = text1 & text2
	intersect.count.to_f / union.count.to_f
end

def jaccardTransform1(index)
	(-Math::log(index,2))+0.5
end

def jaccardTransform(index)
	#((0.25-index)*10)**10
	if index == 0
		-999999999999999
	else
		#if index < 0.4
		#	-jaccardTransform1(index)**10
		#else
			-jaccardTransform1(index)**4
		#end
	end
end

file = File.new(SKUS_INPUT_FILE_NAME, "r")
rawData = file.read
file.close

# parse data
CSV::Converters[:blank_to_nil] = lambda do |field|
  field && field.empty? ? nil : field
end
csv = CSV.new(rawData, :headers => true, :header_converters => :symbol, :converters => [:all, :blank_to_nil])
productArray = csv.to_a.map {|row| row.to_hash }

# url encode brand and lemmatize name
file = File.new(BOW_FILE_NAME,"w")
lem = Lemmatizer.new
productArray.map do |product| 
	brand = [CGI.escape(product[:brand].downcase)]
	product[:bow] = brand + product[:color].downcase.scan(/\w+/)
	productNameTokens = product[:name].downcase.scan(/\w+/)
	productNameTokens.map do |string|
		product[:bow] += [lem.lemma(string)]
	end
end

# remove words that only occur once
# as they make the data set too sparse
counts = Hash.new 0
productArray.each do |product|
	product[:bow].each do |string|
		counts[string] += 1
	end
end

singles = Array.new()
counts.each do |string, count|
	if count == 1
		singles.push(string)
	end
end

productArray.each do |product|
	product[:bow] = product[:bow] - singles
end

# output processed data
productArray.map do |product| 
	file << product[:sku] + ": #{product[:bow]} \n"
end
file.close

# remove if no bag of words
productArray.reject { |product| product[:bow].count == 0 }

# calculate Jaccard Index for similarities
# map to [-inf,0] 
# output with indexes to file
class DebugInfo
	attr_accessor :jaccardIndex
    attr_accessor :invertedJaccardIndex
    attr_accessor :transformedJaccard1Index
    attr_accessor :transformedJaccardIndex
end
file = File.new(CLUSTERER_INPUT_FILE_NAME,"w")
n = productArray.count - 1
numOfComparisons = ((n*n) + n)/2
file << "#{numOfComparisons} #{productArray.count}\n"
debug = Array.new()
productArray.each do |product1|
	productArray.each do |product2|
		i = productArray.index(product1)
		j = productArray.index(product2)
		if j > i
			index = jaccardIndex(product1[:bow],product2[:bow])
			info = DebugInfo.new
			info.jaccardIndex = index
			info.invertedJaccardIndex = 1 - index
			info.transformedJaccard1Index = jaccardTransform1(index)
			info.transformedJaccardIndex = jaccardTransform(index)
			debug.push(info)
			file << "#{i} #{j} #{jaccardTransform(index)}\n"
		end
	end
end
file.close

debug.sort! { |info1, info2| info1.jaccardIndex <=> info2.jaccardIndex }
debug.each { |info| 
	print "#{info.jaccardIndex} ".red
	print "#{info.invertedJaccardIndex} ".blue
	print "#{info.transformedJaccard1Index} ".yellow
	print "#{info.transformedJaccardIndex}\n".green
}


puts "---------------------------------------------------------".blue
puts "--------------------clustering...------------------------".blue
puts "---------------------------------------------------------".blue

# cluster
system("./ap2 #{CLUSTERER_INPUT_FILE_NAME} #{CLUSTERER_OUPUT_FILE_NAME}")
file = File.new(CLUSTERER_OUPUT_FILE_NAME, "r")
clusters = Hash.new
count = 0
file.each_line do |line|
	if !clusters[line.to_i]
		clusters[line.to_i] = [productArray[count]]
	else
		clusters[line.to_i].push(productArray[count])
	end
	count = count + 1
end
file.close

# sort the clusters
class Cluster
	attr_accessor :products
    attr_accessor :keys
end

orderedClusters = Array.new
clusters.each do |index,products|
	cluster = Cluster.new
	cluster.products = products
	# find the keys of each cluster
	allKeyWords = Array.new
	products.each { |product| allKeyWords.concat(product[:bow]) }
	keyWordDictionary = Hash.new(0)
	allKeyWords.each do |keyword|
		keyWordDictionary[keyword] += 1
	end
	# remove all words that occur only once
	# arbitrarily decide the the keys must at least occur half the time
	keyWordDictionary.select! { |keyword, count| count > products.count/2}
	cluster.products = products
	cluster.keys = keyWordDictionary.keys
	orderedClusters.push(cluster)
end

orderedClusters.sort! { |clusterA,clusterB| clusterB.products.count <=> clusterA.products.count }

queries = Array.new
# output the clusters
count = 1
orderedClusters.each do |cluster|
	# decide which clusters to use
	# arbitrarily decide that cluster size has to be large than 2
	if cluster.products.count > 3 && cluster.keys.count > 0
		puts "===========Cluster #{count}================".green
		queries.push(cluster.keys.join(" "))
	else
		puts "===========Cluster #{count}(rejected)======".magenta
	end
	puts "Keys : #{cluster.keys}"
	cluster.products.each do |product|
		puts "#{product[:sku]} #{product[:bow]}"
	end
	count = count+1
end

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
	response = http.get(url,{"query"=>query},{ 'Accept' => 'application/json' })
	if response.status == 200
		productList = JSON.parse(response.body)
		array = productList["data"]["products"]
		array.delete_at(0)
		array.delete_at(0)
		if results.count == 0
		 	results = array
		else
		 	results = results.zip(array).flatten.compact
		end 
		puts "got #{productList["data"]["product_count"]} products for " + query
	else
		puts "get products for #{query} failed"
	end
end

# remove duplicate products
results = results.uniq
# remove products already viewed

# preview everything
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

data = ERB.new(template).result()
file = File.open(PREVIEW_FILE_NAME, "w")
file.write(data)
file.close

system("open -a \"Google Chrome\" #{PREVIEW_FILE_NAME}")