(require 'ox-publish)
(require 'ox-html)


(setq org-publish-project-alist
            '(
	      
	      ;; These are the main web files
	      ("org-notes"
	        :base-directory "." ;; Change this to your local dir
		:base-extension "org"
		:publishing-directory "../website"
		:recursive t
		:publishing-function org-html-publish-to-html
		:headline-levels 4             ; Just the default for this project.
		:auto-preamble t
;;		:section-numbers nil
		:headline-levels 3
		:table-of-contents nil
;;		:style "<link rel='stylesheet' type='text/css' href='css/style.css' />"
		:style-include-default nil
		)

	      ;; These are static files (images, pdf, etc)
	      ("org-static"
	        :base-directory "." ;; Change this to your local dir
		:base-extension "css\\|js\\|png\\|svg\\|jpg\\|gif\\|pdf\\|mp3\\|ogg\\|swf\\|txt\\|asc"
		:publishing-directory "../website"
	        :recursive t
		:publishing-function org-publish-attachment
		)

	      ("org" :components ("org-notes" "org-static"))
	      )
	    )

(defun myweb-publish nil
     "Publish myweb."
     (interactive)
     (org-publish-all))

