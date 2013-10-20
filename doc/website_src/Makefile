EMACS=emacs
ORG_CONFIG_FILE=publish_config.el
EMACS_OPTS=--batch -l ~/.emacs --eval "(load-file \"$(ORG_CONFIG_FILE)\")" -f myweb-publish

DEST_HOST='benjamin@ssh.cs.kuleuven.be:public_html/paraminer'
# DEST_HOST 'bnegreve@paraminer.forge.imag.fr:/var/lib/gforge/chroot/home/groups/paraminer/htdocs'
OUTPUT_DIR=../website

all: html 

html:  ../../README.org include/header.org include/footer.org  main.org counter.php
	@echo "Generating HTML..."
	@mkdir -p $(OUTPUT_DIR)
	@$(EMACS) $(EMACS_OPTS)
	@echo "HTML generation done"

upload: clean_bak
	@cd $(OUTPUT_DIR) && scp -r . $(DEST_HOST) && cd ..


clean: clean_output clean_bak

clean_output:
	@rm -rf $(OUTPUT_DIR)

clean_bak:
	@find . | grep ~$$ | while read l; do rm $$l; done
