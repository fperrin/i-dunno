dist_man_MANS = i-dunno.1 i_dunno_form.3 i_dunno_deform.3

if HAVE_PANDOC
$(dist_man_MANS): $(top_srcdir)/README.md
	$(AWK) "/# $@/{ p=1; sub(/^# /, \"% \");  }		\
		p && /^# / { p=0 }				\
		p { sub(/^##/, \"#\"); print }" < $? |		\
	$(PANDOC) --standalone --to man --output $@

maintainer-clean-local::
	rm -f *.md $(dist_man_MANS)
endif
