# -*- coding: utf-8 -*-

# Copyright (C) 2007-2009,2011 Osmo Salomaa
#
# This file is part of Gaupol.
#
# Gaupol is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Gaupol is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# Gaupol. If not, see <http://www.gnu.org/licenses/>.

"""Automatic correcting of texts."""

import aeidon
import os
import re
import sys
_ = aeidon.i18n._

try:
    import enchant.checker
except Exception:
    pass


class TextAgent(aeidon.Delegate, metaclass=aeidon.Contractual):

    """Automatic correcting of texts."""
    _re_capitalizable = re.compile(r"^\W*(?<!\.\.\.)(?<!…)\w")

    def _capitalize_first(self, parser, pos):
        """
        Capitalize the first alphanumeric character from `pos`.

        Return ``True`` if something was capitalized, ``False`` if not.
        """
        match = self._re_capitalizable.search(parser.text[pos:])
        if match is not None:
            i = pos + match.end() - 1
            prefix = parser.text[:i]
            text = parser.text[i:i + 1].capitalize()
            suffix = parser.text[i + 1:]
            parser.text = prefix + text + suffix
        return match is not None

    def _capitalize_text(self, parser, pattern, cap_next):
        """
        Capitalize all matches of `pattern` in `parser`'s text.

        Return ``True`` if the text of the next subtitle should be capitalized.
        """
        try:
            a, z = parser.next()
        except StopIteration:
            return cap_next
        if pattern.get_field("Capitalize") == "Start":
            self._capitalize_first(parser, a)
        if pattern.get_field("Capitalize") == "After":
            cap_next = not self._capitalize_first(parser, z)
        return self._capitalize_text(parser, pattern, cap_next)

    def _get_enchant_checker_require(self, language):
        assert aeidon.util.enchant_available()

    def _get_enchant_checker(self, language):
        """
        Return an enchant spell-checker for `language`.

        Raise :exc:`enchant.error` if dictionary instatiation fails.
        """
        directory = os.path.join(aeidon.CONFIG_HOME_DIR, "spell-check")
        path = os.path.join(directory, "{}.dict".format(language))
        try: dictionary = enchant.DictWithPWL(language, path)
        except IOError:
            aeidon.util.print_write_io(sys.exc_info(), path)
            dictionary = enchant.Dict(language)
        # Sometimes enchant will initialize a dictionary that will not
        # actually work when trying to use it, hence check something.
        dictionary.check("aeidon")
        return enchant.checker.SpellChecker(dictionary, "")

    def _get_misspelled_indices(self, checker):
        """Return a list of misspelled indices in `checker`'s text."""
        i = [list(range(x.wordpos, x.wordpos + len(x.word))) for x in checker]
        return aeidon.util.flatten(i)

    def _get_penalties_ensure(self, value, patterns):
        assert len(value) <= len(patterns)

    def _get_penalties(self, patterns):
        """Return a list of penalty definitions."""
        return [dict(pattern=x.get_field("Pattern"),
                     flags=x.get_flags(),
                     group=int(x.get_field("Group")),
                     value=float(x.get_field("Penalty"))) for x in patterns]

    def _get_substitutions_ensure(self, value, patterns):
        assert len(value) <= len(patterns)

    def _get_substitutions(self, patterns):
        """Return a sequence of tuples of pattern, flags, replacement."""
        return [(x.get_field("Pattern"),
                 x.get_flags(),
                 x.get_field("Replacement")) for x in patterns]

    def _remove_leftover_hi(self, texts, parser):
        """Remove leftover hearing impaired whitespace and junk."""
        texts = texts[:]
        for i, text in enumerate(texts):
            parser.set_text(text)
            # Remove leading and trailing spaces.
            self._replace_all(parser, r"(^\s+|\s+$)", "")
            # Consolidate multiple consequtive spaces.
            self._replace_all(parser, r" {2,}", " ")
            # Remove lines with no alphanumeric characters.
            self._replace_all(parser, r"^\W*$", "")
            # Remove empty lines.
            self._replace_all(parser, r"(^\n|\n$)", "")
            # Add space after dialogue dashes.
            self._replace_all(parser, r"^([\-\–\—])(\S)", r"\1 \2")
            # Remove dialogue dashes if not present on other lines.
            self._replace_all(parser, r"^[\-\–\—] (.*?^[^\-\–\—])", r"\1")
            # Remove dialogue dashes from single-line subtitles.
            self._replace_all(parser, r"\A[\-\–\—] ([^\n]*)\Z", r"\1")
            texts[i] = parser.get_text()
        return texts

    def _replace_all(self, parser, pattern, replacement):
        """Replace all matches of `pattern` in `parser`'s text."""
        parser.set_regex(pattern)
        parser.replacement = replacement
        parser.replace_all()

    def break_lines_require(self, indices, *args, **kwargs):
        for index in (indices or ()):
            assert 0 <= index < len(self.subtitles)

    @aeidon.deco.export
    @aeidon.deco.revertable
    def break_lines(self,
                    indices,
                    doc,
                    patterns,
                    length_func,
                    max_length,
                    max_lines,
                    skip=False,
                    max_skip_length=2**16,
                    max_skip_lines=2**16,
                    register=-1):

        """
        Break lines to fit defined maximum line length and count.

        `indices` can be ``None`` to process all subtitles. `patterns` should
        be a sequence of instances of :class:`aeidon.Pattern`. `length_func`
        should return the length of a string argument. `max_length` should be
        the maximum allowed length of lines in the same scale as returned by
        `length_func`. `max_lines` may be violated to avoid violating
        `max_length`. If `skip` is ``True``, subtitles that do not violate or
        do not manage to reduce `max_skip_length` and `max_skip_lines` are
        skipped.

        Raise :exc:`re.error` if a bad regular expression among `patterns`.
        """
        new_indices = []
        new_texts = []
        patterns = [x for x in patterns if x.enabled]
        penalties = self._get_penalties(patterns)
        liner = self.get_liner(doc)
        liner.set_penalties(penalties)
        liner.length_func = length_func
        liner.max_length = max_length
        liner.max_lines = max_lines
        re_tag = self.get_markup_tag_regex(doc)
        for index in indices or self.get_all_indices():
            subtitle = self.subtitles[index]
            liner.set_text(subtitle.get_text(doc))
            plain_text = subtitle.get_text(doc)
            if re_tag is not None:
                plain_text = re_tag.sub("", plain_text)
            lines = plain_text.split("\n")
            length = max(map(length_func, lines))
            line_count = len(lines)
            if length <= max_skip_length:
                if line_count <= max_skip_lines:
                    # Skip subtitles that do not violate
                    # any of the defined skip conditions.
                    if skip: continue
            text = liner.break_lines()
            if re_tag is not None:
                plain_text = re_tag.sub("", text)
            lines = plain_text.split("\n")
            length_down = max(map(length_func, lines)) < length
            lines_down = len(lines) < line_count
            length_fixed = length > max_skip_length and length_down
            lines_fixed = line_count > max_skip_lines and lines_down
            if not length_fixed and not lines_fixed:
                # Skip if part in violation not fixed.
                if skip: continue
            if text != subtitle.get_text(doc):
                new_indices.append(index)
                new_texts.append(text)
        if not new_indices: return
        self.replace_texts(new_indices, doc, new_texts, register=register)
        self.set_action_description(register, _("Breaking lines"))

    def capitalize_require(self, indices, doc, pattern, register=-1):
        for index in (indices or ()):
            assert 0 <= index < len(self.subtitles)

    @aeidon.deco.export
    @aeidon.deco.revertable
    def capitalize(self, indices, doc, patterns, register=-1):
        """
        Capitalize texts as defined by `patterns`.

        `indices` can be ``None`` to process all subtitles. `patterns` should
        be a sequence of instances of :class:`aeidon.Pattern`. Raise
        :exc:`re.error` if a bad regular expression among `patterns`.
        """
        new_indices = []
        new_texts = []
        parser = self.get_parser(doc)
        patterns = [x for x in patterns if x.enabled]
        indices = indices or self.get_all_indices()
        for indices in aeidon.util.get_ranges(indices):
            cap_next = False
            for index in indices:
                subtitle = self.subtitles[index]
                parser.set_text(subtitle.get_text(doc))
                if cap_next or (index == 0):
                    self._capitalize_first(parser, 0)
                    cap_next = False
                for pattern in patterns:
                    string = pattern.get_field("Pattern")
                    flags = pattern.get_flags()
                    parser.set_regex(string, flags)
                    parser.pos = 0
                    cap_next = self._capitalize_text(parser,
                                                     pattern,
                                                     cap_next)

                text = parser.get_text()
                if text != subtitle.get_text(doc):
                    new_indices.append(index)
                    new_texts.append(text)
        if not new_indices: return
        self.replace_texts(new_indices, doc, new_texts, register=register)
        self.set_action_description(register, _("Capitalizing texts"))

    def correct_common_errors_require(self, indices, *args, **kwargs):
        for index in (indices or ()):
            assert 0 <= index < len(self.subtitles)

    @aeidon.deco.export
    @aeidon.deco.revertable
    def correct_common_errors(self, indices, doc, patterns, register=-1):
        """
        Correct common human and OCR errors in texts.

        `indices` can be ``None`` to process all subtitles. `patterns` should
        be a sequence of instances of :class:`aeidon.Pattern`. Raise
        :exc:`re.error` if a bad regular expression among `patterns`.
        """
        new_indices = []
        new_texts = []
        parser = self.get_parser(doc)
        patterns = [x for x in patterns if x.enabled]
        re_patterns = self._get_substitutions(patterns)
        repeats = [x.get_field_boolean("Repeat") for x in patterns]
        for index in indices or self.get_all_indices():
            subtitle = self.subtitles[index]
            parser.set_text(subtitle.get_text(doc))
            for i, (string, flags, replacement) in enumerate(re_patterns):
                parser.set_regex(string, flags)
                parser.replacement = replacement
                count = parser.replace_all()
                while repeats[i] and count:
                    count = parser.replace_all()
            text = parser.get_text()
            if text != subtitle.get_text(doc):
                new_indices.append(index)
                new_texts.append(text)
        if not new_indices: return
        self.replace_texts(new_indices, doc, new_texts, register=register)
        self.set_action_description(register, _("Correcting common errors"))

    def remove_hearing_impaired_require(self, indices, *args, **kwargs):
        for index in (indices or ()):
            assert 0 <= index < len(self.subtitles)

    @aeidon.deco.export
    @aeidon.deco.revertable
    def remove_hearing_impaired(self, indices, doc, patterns, register=-1):
        """
        Remove hearing impaired parts from subtitles.

        `indices` can be ``None`` to process all subtitles. `patterns` should
        be a sequence of instances of :class:`aeidon.Pattern`. Raise
        :exc:`re.error` if a bad regular expression among `patterns`.
        """
        new_indices = []
        new_texts = []
        parser = self.get_parser(doc)
        patterns = [x for x in patterns if x.enabled]
        re_patterns = self._get_substitutions(patterns)
        for index in indices or self.get_all_indices():
            subtitle = self.subtitles[index]
            parser.set_text(subtitle.get_text(doc))
            for string, flags, replacement in re_patterns:
                parser.set_regex(string, flags)
                parser.replacement = replacement
                parser.replace_all()
            text = parser.get_text()
            if text != subtitle.get_text(doc):
                new_indices.append(index)
                new_texts.append(text)
        if not new_indices: return
        new_texts = self._remove_leftover_hi(new_texts, parser)
        self.replace_texts(new_indices, doc, new_texts, register=register)
        description = _("Removing hearing impaired texts")
        self.set_action_description(register, description)
        remove_indices = []
        for i, text in (x for x in enumerate(new_texts) if not x[1]):
            remove_indices.append(new_indices[i])
        if not remove_indices: return
        self.remove_subtitles(remove_indices, register=register)
        self.group_actions(register, 2, description)

    def spell_check_join_words_require(self, indices, *args, **kwargs):
        for index in (indices or ()):
            assert 0 <= index < len(self.subtitles)
        assert aeidon.util.enchant_available()

    @aeidon.deco.export
    @aeidon.deco.revertable
    def spell_check_join_words(self, indices, doc, language, register=-1):
        """
        Join misspelled words based on spell-checker suggestions.

        Raise :exc:`enchant.Error` if dictionary instatiation fails.
        """
        new_indices = []
        new_texts = []
        re_multispace = re.compile(r" +")
        checker = self._get_enchant_checker(language)
        seeker = self._get_enchant_checker(language)
        for index in indices or self.get_all_indices():
            subtitle = self.subtitles[index]
            text = subtitle.get_text(doc)
            text = re_multispace.sub(" ", text)
            checker.set_text(text)
            while True:
                try: next(checker)
                except StopIteration: break
                text = checker.get_text()
                a = checker.wordpos
                z = checker.wordpos + len(checker.word)
                ok_with_prev = ok_with_next = False
                if checker.leading_context(1) == " ":
                    seeker.set_text(text[:a - 1] + text[a:])
                    poss = self._get_misspelled_indices(seeker)
                    ok_with_prev = not (a - 1) in poss
                if checker.trailing_context(1) == " ":
                    seeker.set_text(text[:z] + text[z + 1:])
                    poss = self._get_misspelled_indices(seeker)
                    ok_with_next = not a in poss
                # Join backwards or forwards if only one direction,
                # but not both, produce a correctly spelled result.
                if ok_with_prev and not ok_with_next:
                    checker.set_text(text[:a - 1] + text[a:])
                if ok_with_next and not ok_with_prev:
                    checker.set_text(text[:z] + text[z + 1:])
            new_text = checker.get_text()
            if new_text != text:
                new_indices.append(index)
                new_texts.append(new_text)
        if not new_indices: return
        self.replace_texts(new_indices, doc, new_texts, register=register)
        description = _("Joining words by spell-check suggestions")
        self.set_action_description(register, description)

    def spell_check_split_words_require(self, indices, *args, **kwargs):
        for index in (indices or ()):
            assert 0 <= index < len(self.subtitles)
        assert aeidon.util.enchant_available()

    @aeidon.deco.export
    @aeidon.deco.revertable
    def spell_check_split_words(self, indices, doc, language, register=-1):
        """
        Split misspelled words based on spell-checker suggestions.

        Using this is usually not a good idea unless you have an insane
        dictionary that contains all possible compound words in `language`.
        Raise :exc:`enchant.Error` if dictionary instatiation fails.
        """
        new_indices = []
        new_texts = []
        re_multispace = re.compile(r" +")
        checker = self._get_enchant_checker(language)
        indices = indices or self.get_all_indices()
        for index in indices:
            subtitle = self.subtitles[index]
            text = subtitle.get_text(doc)
            text = re_multispace.sub(" ", text)
            checker.set_text(text)
            while True:
                try: next(checker)
                except StopIteration: break
                if checker.word.capitalize() == checker.word:
                    # Skip capitalized words, which are usually names
                    # and thus not always found in dictionaries.
                    continue
                suggestions = []
                for i, suggestion in enumerate(checker.suggest()):
                    if suggestion.find(" ") > 0:
                        if suggestion.replace(" ", "") == checker.word:
                            suggestions.append(suggestion)
                # Split word only if only one two-word suggestion found that
                # has all the same characters as the original unsplit word.
                if len(suggestions) != 1: continue
                text = checker.get_text()
                a = checker.wordpos
                z = checker.wordpos + len(checker.word)
                checker.set_text(text[:a] + suggestions[0] + text[z:])
            new_text = checker.get_text()
            if new_text != text:
                new_indices.append(index)
                new_texts.append(new_text)
        if not new_indices: return
        self.replace_texts(new_indices, doc, new_texts, register=register)
        description = _("Splitting words by spell-check suggestions")
        self.set_action_description(register, description)
